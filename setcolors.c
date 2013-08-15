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

	unsigned char red;
	unsigned char green;
	unsigned char blue;

	int i, k;
	for (i = k = 0; i < 16; ++i)
	{
		red   = test.colors[k++];
		green = test.colors[k++];
		blue  = test.colors[k++];

		printf("%d is #%02X%02X%02X\n", i, red, green, blue);
	}

	close(fd);

	return 0;
}
