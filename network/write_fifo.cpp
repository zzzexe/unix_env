#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    char buf[] = "fifo test\n";
    int fd = open("test_fifo", O_WRONLY);
    if (fd < 0) {
        std::cerr << "open fifo error" << std::endl;
    }
    while (true) {
        int res = write(fd, buf, sizeof(buf));
        if (res == -1) {
            std::cerr << "write error" << std::endl;
        }
        sleep(3);
    }
    close(fd);
    return 0;
}
