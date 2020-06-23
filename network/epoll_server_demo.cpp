/*
 * I/O多路复用demo，注册stdin和fifo两个事件
 *
 * epoll相关的结构定义：
 * typedef union epoll_data {
 *     void *ptr;
 *     int fd;
 *     _uint32_t u32;
 *     _uint64_t u64;
 * } epoll_data_t;
 *
 * struct epoll_event {
 *     _unit32_t events;
 *     epoll_data_t data;
 * };
 *
 * epoll相关系统调用：
 * int epoll_create(int size);
 * int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
 * int epoll_wait(int epfd, struct epoll_event* events, int max_events, int timeout);
 */
#include <iostream>
#include <sys/epoll.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    // make fifo
    int ret = mkfifo("test_fifo", 0666);
    if (ret != 0) {
        std::cerr << "mkfifo error" << std::endl;
    }
    // open fifo，此处需要读、写打开
    int fd = open("test_fifo", O_RDONLY);
    if (fd < 0) {
        std::cerr << "open fifo error" << std::endl;
        return -1;
    }
    std::cout << "fifo_fd=" << fd << std::endl;

    // add epoll event
    struct epoll_event event;
    int epfd = epoll_create(10);
    if (epfd == -1) {
        std::cerr << "epoll_create error" << std::endl;
        return -1;
    }
    event.data.fd = 0;
    event.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &event);
    if (ret == -1) {
        std::cerr << "epoll_ctl error" << std::endl;
        return -1;
    }
    event.data.fd = fd;
    event.events = EPOLLIN;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    if (ret == -1) {
        std::cerr << "epoll_ctl error" << std::endl;
        return -1;
    }

    struct epoll_event wait_event;
    while (true) {
        std::cerr << "epoll wait..." << std::endl;
        ret = epoll_wait(epfd, &wait_event, 2, -1);
        if (ret == -1) {
            close(epfd);
            std::cerr << "epoll error" << std::endl;
        } else if (ret > 0) {
            char buf[100] = {0};
            if ( (wait_event.data.fd == 0) && (wait_event.events == EPOLLIN & EPOLLIN) ) {
                read(0, buf, sizeof(buf));
                printf("stdin buf=%s\n", buf);
            } else if ( (wait_event.data.fd == fd) && (wait_event.events == EPOLLIN & EPOLLIN) ) {
                read(fd, buf, sizeof(buf));
                printf("fifo buf=%s", buf);
            }
        } else if (ret == 0) {
            printf("time out\n");
        }
    }
    close(epfd);
    return 0;
}
