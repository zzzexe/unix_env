#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#define PAGER "${PAGER:-more}"

const int MAXLINE = 4096;

int main (int argc, char **argv) {
    char line[MAXLINE];
    FILE *fpin, *fpout;

    if (argc != 2) {
        std::cerr << "usage: a.out <pathname>" << std::endl;
        exit(-1);
    }

    if ( (fpin = fopen(argv[1], "r")) == NULL ) {
        std::cerr << "can not open " << argv[1] << std::endl;
    }
    if ( (fpout = popen(PAGER, "w")) == NULL ) {
        std::cerr << "popen error" << std::endl;
    }

    while (fgets(line, MAXLINE, fpin) != NULL) {
        if (fputs(line, fpout) == EOF) {
            std::cerr << "fputs error to pipe" << std::endl;
        }
    }

    if (ferror(fpin)) {
        std::cerr << "fgets error" << std::endl;
    }
    if (pclose(fpout) == -1) {
        std::cerr << "pclose error" << std::endl;
    }
    exit(0);
}
