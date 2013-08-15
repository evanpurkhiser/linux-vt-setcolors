#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>


int main(int argc, char *argv[])
{
	int fd = open("/dev/console", O_RDWR);
	
	struct colors { unsigned char colors[48]; };
	struct colors test;

	ioctl(fd, GIO_CMAP, &test);


	close(fd);

	return 0;
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
