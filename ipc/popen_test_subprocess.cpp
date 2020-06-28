#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>

const int MAXLINE = 4096;

int main() {
    std::cout << "subprocess start" << std::endl;
    /*
    char buf[MAXLINE];
    while (true) {
        size_t ret = read(1, buf, sizeof(buf));
        if (ret) {
            printf("read data from stdin, data=%s", buf);
        } else {
            std::cerr << "subprocess read error" << std::endl;
        }
    }
    */
    std::string line;
    while (std::getline(std::cin, line)) {
        std::cout << "received data" << std::endl;
        std::cout << "read data from stdin, line=" << line << std::endl;
    }
    return 0;
}
