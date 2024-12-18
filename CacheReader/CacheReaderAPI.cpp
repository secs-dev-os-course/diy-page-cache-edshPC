#include "CacheReaderAPI.h"

#include "CacheReader.h"

#define CATCH_N_PRINT catch (const std::exception &e) { \
        std::cerr << e.what() << std::endl;             \
        return -1; }

namespace os {
    static std::unordered_map<int, CacheReader *> readers;
    static int next_fd = 1;
}

// Открытие файла по заданному пути файла, доступного для чтения
int lab2_open(int *fd, const char *path) {
    try {
        if (!fd || !path) return -1;
        auto reader = new os::CacheReader(path);
        reader->open();
        *fd = os::next_fd++;
        os::readers[*fd] = reader;
        return 0;
    } CATCH_N_PRINT;
}

// Закрытие файла по хэндлу
int lab2_close(int fd) {
    try {
        auto reader = os::readers.at(fd);
        reader->close();
        os::readers.erase(fd);
        delete reader;
        return 0;
    } CATCH_N_PRINT;
}

// Чтение данных из файла
int lab2_read(int fd, void *buf, size_t count, size_t *read) {
    try {
        if (!buf) return -1;
        auto reader = os::readers.at(fd);
        size_t res = reader->read(buf, count);
        if (read) *read = res;
        return 0;
    } CATCH_N_PRINT;
}

// Запись данных в файл
int lab2_write(int fd, const void *buf, size_t count, size_t *written) {
    try {
        if (!buf) return -1;
        auto reader = os::readers.at(fd);
        size_t res = reader->write(buf, count);
        if (written) *written = res;
        return 0;
    } CATCH_N_PRINT;
}

// Перестановка позиции указателя на данные файла
off_t lab2_lseek(int fd, off_t offset, int whence) {
    try {
        auto reader = os::readers.at(fd);
        return reader->lseek(offset, whence);
    } CATCH_N_PRINT;
}

// Синхронизация данных из кэша с диском
int lab2_fsync(int fd) {
    try {
        auto reader = os::readers.at(fd);
        reader->fsync();
        return 0;
    } CATCH_N_PRINT;
}
