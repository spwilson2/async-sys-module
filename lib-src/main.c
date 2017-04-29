#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>


int main(void) {
    int fd = open("/dev/as-sys", 0);
    if (fd < 0)
        printf("Unable to open dev file\n");
    ioctl(fd, 0);
    return 0;
}
