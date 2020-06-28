#include <iostream>
#include <string>
#include <random>
#include <unistd.h>

int main() {
    std::cout << "subprocess starting..." << std::endl;
    srand((unsigned int)(time(NULL)));
    // receive packet
    std::string line = "";
    while (std::getline(std::cin, line)) {
        std::cout << "subprocess, receive data from parent, data=" << line << std::endl;
        // mock processing...
        std::random_device rd;
        int sec = rd();
        if (sec < 0) {
            sec = sec * -1;
        }
        sec = sec % 7;
        sleep(sec);
        // send request
        std::string::size_type pos = line.find("packet_id");
        std::string packet_id = line.substr(pos + 10, 8);
        std::string result = "packet process success, packet_id=" + packet_id + ", latency=" + std::to_string(sec) + "\n";
        size_t ret = write(0, result.c_str(), result.size());
    }
    std::cout << "subprocess exiting..." << std::endl;
    return 0;
}
