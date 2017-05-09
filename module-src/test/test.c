#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const static char fname[] = "/dev/as_sys";
	int fd;
	if( access(fname , F_OK ) != -1 ) {
		fd = open(fname, O_RDWR);
	} else {
		printf("FAILED TO FIND FILE\n");
	}
	printf("fd: %d\n", fd);
	if (fd) {
		ioctl(fd, _IOWR(22,2,sizeof(int)), (int)22, (int)33);
	} else {
		printf("FAILED TO OPEN FILE\n");
	}
	return (!fd);
}
