/*
 * 进程间通信demo
 * main_process持续向subprocess传送数据
 * subprocess接收数据以random延迟告知main_process
 * 使用socketpair系统调用，并调用dup2将subprocess的标准输入设置为socket pair的另一端
 * main_process使用epoll对subprocess发来的数据进行监听
 */

#include <iostream>
#include <string>
#include <random>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>

const int MAXLINE = 1024;
struct epoll_event wait_event;
int epfd = -1;
int child = -1;
int parent = -1;

void* thread_proc(void* args) {
    while (true) {
        int ret = epoll_wait(epfd, &wait_event, 2, -1);
        if (ret == -1) {
            std::cerr << "epoll wait error" << std::endl;
            close(epfd);
            break;
        } else if (ret > 0) {
            char buf[MAXLINE] = {0};
            if ( (wait_event.data.fd == parent) && (wait_event.events == EPOLLIN & EPOLLIN) ) {
                read(parent, buf, sizeof(buf));
                printf("main process, receive data from subprocess, data=%s", buf);
            }
        } else if (ret == 0) {
            std::cerr << "epoll_wait timed out" << std::endl;
        }
    }
    std::cerr << "epoll wait thread exiting..." << std::endl;
    close(epfd);
}

int main(int argc, char **argv) {
    srand((unsigned int)(time(NULL)));
    pid_t pid;
    int fd[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fd) < 0) {
        std::cerr << "socketpair error" << std::endl;
        exit(-1);
    }
    parent = fd[0];
    child = fd[1];

    if ( (pid = fork()) < 0 ) {
        std::cerr << "fork error" << std::endl;
        close(parent);
        close(child);
        exit(-1);
    }

    if (pid == 0) {
        // subprocess
        close(parent);
        if (child != STDIN_FILENO) {
            if (dup2(child, STDIN_FILENO) < 0) {
                close(child);
                exit(-1);
            }
        }
        close(child);
        char *args[] = {"./socket_pair_subprocess", (char *)0};
        char *envp[] = {0};
        if (execve("./socket_pair_subprocess", args, envp) < 0) {
            std::cerr << "child process exec failed" << std::endl;
            exit(-1);
        }
    } else {
        // parent process
        close(child);
        sleep(1);
        // add epoll event for parent fd
        struct epoll_event event;
        epfd = epoll_create(1);
        if (epfd == -1) {
            std::cerr << "epoll_create error" << std::endl;
            exit(-1);
        }
        event.data.fd = parent;
        event.events = EPOLLIN;
        int epoll_ret = epoll_ctl(epfd, EPOLL_CTL_ADD, parent, &event);
        if (epoll_ret == -1) {
            std::cerr << "epoll_ctl error" << std::endl;
            exit(-1);
        }
        // new thread for epoll wait
        pthread_t tid;
        int thread_ret = pthread_create(&tid, NULL, thread_proc, NULL);
        if (thread_ret != 0) {
            std::cerr << "pthread create error" << std::endl;
            exit(-1);
        }
        // send data to subprocess
        while (true) {
            std::string packet_id = "00000000";
            for (int i = 0; i < static_cast<int>(packet_id.size()); i++) {
                std::random_device rd;
                int index = rd();
                if (index < 0) {
                    index = index * -1;
                }
                index = index % 9;
                packet_id[i] = '0' + index;
            }
            std::string packet = "{\"packet_id\":\"" + packet_id + "\", \"url\":\"https://www.toutiao.com/\"}\n";
            int ret = write(parent, packet.c_str(), packet.size());
            sleep(3);
        }
        std::cerr << "main process exiting..." << std::endl;
    }

    exit(0);
}
