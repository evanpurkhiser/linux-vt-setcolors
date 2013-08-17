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

static const char default_color_set[PALETTE_SIZE][7] = {
	"000000","aa0000","00aa00","aa5500",
	"0000aa","aa00aa","00aaaa","aaaaaa",
	"555555","ff5555","55ff55","ffff55",
	"5555ff","ff55ff","55ffff","ffffff" };

/**
 * The palette struct is the type epxected by ioctl PIO_CMAP
 */
struct palette { unsigned char colors[PALETTE_SIZE * 7]; };

/**
 * Convert a list of colors in hex format to their actual hex formats suitable
 * for passing to the ioctl function
 */
static struct palette
get_palette_from_color_set(const char colors[][7])
{
	struct palette palette;
	unsigned int red, green, blue;
	int i, k;

	for (i = k = 0; i < PALETTE_SIZE; ++i)
	{
		if (sscanf(colors[i], "%2x%2x%2x", &red, &green, &blue) != 3)
		{
			fprintf(stderr, "Color %d: Invalid hex code\n", i);
			exit(1);
		}

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
	char type = NULL;

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

	// Make sure the tty is a linux VT101 terminal
	if ( ! isatty(fd) || ioctl(fd, KDGKBTYPE, &type) < 0 || type != KB_101)
		return -1;

	return fd;
}

/**
 * Read color strings in from a file terminated by new lines
 */
static int
get_color_set_from_file(const char *file_path, char color_set[][7])
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

		if (color[0] == '#')
			++color;

		if (strlen(color) < 7)
		{
			fprintf(stderr, "Color %d: value too short, skipping\n", i);
			continue;
		}

		// color_set will already have null terminating characters at index 7
		strncpy(color_set[i], color, 6);
	}

	if (line)
		free(line);

	fclose(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	// Default the color set to the default colors if none specified
	char color_set[PALETTE_SIZE][7];
	memcpy(color_set, default_color_set, PALETTE_SIZE * 7);

	// By default let the console path be detected
	char *console_path = NULL;

	if (get_color_set_from_file("/home/evan/test.txt", color_set) < 0)
	{
		perror("Unable to open colors file");
		exit(1);
	}

	struct palette new_palette = get_palette_from_color_set((const char (*)[]) color_set);
	int fd = get_console_fd(console_path);

	if (ioctl(fd, PIO_CMAP, &new_palette) < 0)
	{
		perror("Failed to set new color map on console");
		exit(1);
	}

	// Clear console to fix color artifacts
	write(fd, "\033[2J\033[1;1H", 10);

	close(fd);
	return 0;
}
