#include <iostream>
#include <vector>

#include "CacheReader/CacheReaderAPI.h"

int main() {
    int fd;
    lab2_open(&fd, "test.txt");
    char c;
    size_t read;
    lab2_read(fd, &c, 1, &read);
    while (read) {
        std::cout << c;
        lab2_read(fd, &c, 1, &read);
    }
    lab2_close(fd);

    lab2_open(&fd, "test.txt");
    char buf[10000];
    lab2_read(fd, buf, 9999, &read);
    buf[read] = 0;
    std::cout << buf;
    std::string write_str = "Hello World!\n";
    lab2_write(fd, write_str.c_str(), write_str.size(), nullptr);
    lab2_close(fd);
    return 0;
}
