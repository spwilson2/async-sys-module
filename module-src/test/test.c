#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/syscall.h>

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
		ioctl(fd, _IOWR(SYS_exit,2,sizeof(int)), (int)SYS_exit, (int)33);
		printf("Didn't exit...\n");
	} else {
		printf("FAILED TO OPEN FILE\n");
	}
	return (!fd);
}
