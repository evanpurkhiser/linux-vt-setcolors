#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>

#define PALETTE_SIZE 16

static const char *console_paths[] = {
	"/proc/self/fd/0",
	"/dev/tty",
	"/dev/tty0",
	"/dev/vc/0",
	"/dev/systty",
	"/dev/console",
	NULL
};

static const char default_color_set[PALETTE_SIZE][6] = {
	"000000","aa0000","00aa00","aa5500",
	"0000aa","aa00aa","00aaaa","aaaaaa",
	"555555","ff5555","55ff55","ffff55",
	"5555ff","ff55ff","55ffff","ffffff" };

/**
 * The palette struct is the type epxected by ioctl PIO_CMAP
 */
struct palette { unsigned char colors[PALETTE_SIZE * 16]; };

/**
 * Convert a list of colors in hex format to their actual hex formats suitable
 * for passing to the ioctl function
 */
static struct palette
get_palette_from_color_set(const char colors[][6])
{
	struct palette palette;
	unsigned int red, green, blue;
	int i, k;

	for (i = k = 0; i < PALETTE_SIZE; ++i)
	{

		if (sscanf(colors[i], "%2x%2x%2x", &red, &green, &blue) != 3)
			perror("Malformed hex color code");

		palette.colors[k++] = red;
		palette.colors[k++] = green;
		palette.colors[k++] = blue;
	}

	return palette;
}

/**
 * Get a file descriptor for a console to write to
 */
static int
get_console_fd(const char *console_path)
{
	int i, fd;

	// Use one of the default console paths
	if ( ! console_path)
	{
		for (i = 0; console_paths[i]; ++i)
		{
			if ((fd = get_console_fd(console_paths[i])) > 0)
				return fd;
		}

		return -1;
	}

	// Attempt to open the FD, and make sure it's a tty
	if ((fd = open(console_path, O_RDWR | O_NOCTTY)) < 0)
		return -1;

	return fd;
}

/**
 * Read color strings in from a file terminated by new lines
 */
static int
get_color_set_from_file(const char *file_path, char color_set[][6])
{
	int i;
	FILE *fd;
	char *line, *color = NULL;
	size_t read;

	if ((fd = fopen(file_path, "r")) < 0)
		return -1;

	for (i = 0; getline(&line, &read, fd) > 0; ++i)
	{
		// Don't read more colors than we can fit in our color set
		if ( ! color_set[i])
			break;

		color = line;

		// Drop the hex from the color code
		if (color[0] == '#')
			++color;

		strncpy(color_set[i], color, 6);
	}

	free(line);
	fclose(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	// Default the color set to the default colors if none specified
	char color_set[PALETTE_SIZE][6];
	memcpy(color_set, default_color_set, PALETTE_SIZE * 6);

	if (ioctl(fd, PIO_CMAP, &new_colors) < 0)
		perror("Failed to set new color map on console");

	close(fd);
	return 0;
}
