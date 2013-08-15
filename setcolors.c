#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

struct palette { unsigned char colors[48]; };

/**
 * Convert a list of colors in hex format to their actual hex formats suitable
 * for passing to the ioctl function
 */
struct palette
get_palette_from_hex_set(const char *colors[])
{
	struct palette palette;
	unsigned int red, green, blue;
	int i, k;

	for (i = k = 0; i < 16; ++i)
	{
		sscanf(colors[i], "%2x%2x%2x", &red, &green, &blue);

		palette.colors[k++] = red;
		palette.colors[k++] = green;
		palette.colors[k++] = blue;
	}

	return palette;
}


void print_colors(unsigned char colors[])
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	int i, k;
	for (i = k = 0; i < 16; ++i)
	{
		red   = colors[k++];
		green = colors[k++];
		blue  = colors[k++];

		printf("%02d is #%02X%02X%02X\n", i, red, green, blue);
	}
}

int main(int argc, char *argv[])
{
	const char *colors = "000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";

	struct colors new_colors;

	unsigned char *dst = new_colors.values;
	unsigned char *end = new_colors.values + sizeof(new_colors.values);
	unsigned int u;

	while (dst < end && sscanf(colors, "%2x", &u) == 1)
	{
		*dst++ = u;
		colors += 2;
	}

	int fd = open("/dev/console", O_NOCTTY);
	int stat = ioctl(fd, PIO_CMAP, &new_colors);
	close(fd);

	printf("status: %d == %d == %d\n", stat, errno, EFAULT);

	return 0;
}
