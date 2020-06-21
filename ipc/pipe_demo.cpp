/*
 * 创建从父进程到子进程的管道，父进程通过管道向子进程传送数据
 */


#include <stdio.h>
#include <unistd.h>
#include <iostream>

const int MAXLINE = 4096;

int main() {
    int fd[2];
    pid_t pid;
    char line[MAXLINE];

    if (pipe(fd) < 0) {
        std::cerr << "pipe error" << std::endl;
    }

    if ((pid = fork()) < 0) {
        std::cerr << "fork error" << std::endl;
    } else if (pid > 0) {
        close(fd[0]);
        write(fd[1], "hello from your father\n", 23);
    } else {
        close(fd[1]);
        std::cout << "this is child " << pid << ", read data from father" << std::endl;
        int n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }

    return 0;
}
