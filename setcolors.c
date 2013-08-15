#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/kd.h>


int main(int argc, char *argv[])
{
	int fd = open("/dev/console", O_RDWR);
	
	struct colors { char colors[48]; };
	struct colors test;

	int stat = ioctl(fd, GIO_CMAP, &test);

	printf("Return: %d, val: %s\n", stat, test.colors[5]);

	close(fd);
}
