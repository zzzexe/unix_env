/*
 * 尝试使用popen调用实现进程间通信
 * popen_test_main持续向popen_test_subprocess写消息，popen_test_subprocess收到后print
 * 尝试失败，popen_test_subprocess使用read和getline均未打印出收到的消息
 */
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    FILE *fpout;
    // if ( (fpout = popen("/home/zhaozhao.30/repos/unix_env/ipc/popen_test_subprocess", "w")) == NULL ) {
    if ( (fpout = popen("./popen_test_subprocess", "w")) == NULL ) {
        std::cerr << "popen error" << std::endl;
        exit(-1);
    }
    std::cout << "popen success" << std::endl;
    sleep(1);
    while (true) {
        std::string packet = "{\"url\":\"https://www.toutiao.com/\"}\n\0";
        int ret = fputs(packet.c_str(), fpout);
        std::cout << "ret=" << ret << std::endl;
        sleep(5);
    }
    exit(0);
}
