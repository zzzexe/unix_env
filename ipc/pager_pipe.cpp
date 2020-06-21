#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#define DEF_PAGER "/usr/bin/more"

const int MAXLINE = 4096;

int main(int argc, char **argv) {
    int fd[2];
    pid_t pid;
    char *pager, *argv0;
    char line[MAXLINE];
    FILE *fp;

    if (argc != 2) {
        std::cerr << "usage: a.out <pathname>" << std::endl;
        exit(-1);
    }

    // open file
    if ( (fp=fopen(argv[1], "r")) == NULL ) {
        std::cerr << "can not open " << argv[1] << std::endl;
    }

    // create pipe
    if (pipe(fd) < 0) {
        std::cerr << "pipe error" << std::endl;
    }

    if ( (pid = fork()) < 0 ) {
        std::cerr << "fork error" << std::endl;
    } else if (pid > 0) {
        close(fd[0]);
        while (fgets(line, MAXLINE, fp) != NULL) {
            int n = strlen(line);
            if (write(fd[1], line, n) != n) {
                std::cerr << "write error to pipe" << std::endl;
            }
        }
        if (ferror(fp)) {
            std::cerr << "fgets error" << std::endl;
        }
        close(fd[1]);
        if (waitpid(pid, NULL, 0) < 0) {
            std::cerr << "waitpid error" << std::endl;
        }
        exit(0);
    } else {
        close(fd[1]);
        if (fd[0] != STDIN_FILENO) {
            if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO) {
                std::cerr << "dup2 error to stdin" << std::endl;
            }
            close(fd[0]);
        }
        if ( (pager = getenv("PAGER")) == NULL ) {
            pager = DEF_PAGER;
        }
        if ((argv0 = strrchr(pager, '/')) != NULL) {
            argv0++;
            std::cout << "argv0++, argv0=" << argv0 << std::endl;
        } else {
            argv0 = pager;
            std::cout << argv0 << std::endl;
        }

        if (execl(pager, argv0, (char *)0) < 0) {
            std::cerr << "execl error for " << pager << std::endl;
        }
    }
    exit(0);
}
