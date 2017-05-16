#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/syscall.h>
#include "../../include/as_sys/ioctl.h"

int main(void) {
    const static char fname[] = "/dev/as_sys";
    async_context_t ctx_id;
    struct _async_setup async_setup_args = {.nr_events = 1, .ctx_idp = &ctx_id};
	int fd;
	if( access(fname , F_OK ) != -1 ) {
		fd = open(fname, O_RDWR);
	} else {
		printf("FAILED TO FIND FILE\n");
	}
	printf("fd: %d\n", fd);
	if (fd > 0) {
		// Should fail syscall because bad magic header.
		ioctl(fd, _IOWR(SYS_exit,2,sizeof(int)), (int)SYS_exit, (int)33);
		ioctl(fd, AS_SYS_SETUP, &async_setup_args);

	} else {
		printf("FAILED TO OPEN FILE\n");
	}
	return (!fd);
}
