#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

#define NUM_COLORS 16

static const char *default_palette[16] = {
"000000","aa0000","00aa00","aa5500","0000aa",
"aa00aa","00aaaa","aaaaaa","555555","ff5555",
"55ff55","ffff55","5555ff","ff55ff","55ffff","ffffff" };

struct palette { unsigned char colors[NUM_COLORS * 16]; };

/**
 * Convert a list of colors in hex format to their actual hex formats suitable
 * for passing to the ioctl function
 */
static struct palette
get_palette_from_hex_set(const char *colors[])
{
	struct palette palette;
	unsigned int red, green, blue;
	int i, k;

	for (i = k = 0; i < NUM_COLORS; ++i)
	{
		if (sscanf(colors[i], "%2x%2x%2x", &red, &green, &blue) != 3)
			perror("Malformed hex color code");

		palette.colors[k++] = red;
		palette.colors[k++] = green;
		palette.colors[k++] = blue;
	}

	return palette;
}

int main(int argc, char *argv[])
{
	struct palette new_colors = get_palette_from_hex_set(default_palette);

	int fd = open("/dev/console", O_NOCTTY);

	if (ioctl(fd, PIO_CMAP, &new_colors) < 0)
		perror("Failed to set new color map on console");

	close(fd);
	return 0;
}
