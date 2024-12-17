#include "CacheReaderAPI.h"

#include <cstdio>

// Открытие файла по заданному пути файла, доступного для чтения
int lab2_open(const char *path) {
    printf("Opening %s\n", path);
    return 0;
}

// Закрытие файла по хэндлу
int lab2_close(int fd) { return 0; }

// Чтение данных из файла
int lab2_read(int fd, void *buf, size_t count, size_t *read) { return 0; }

// Запись данных в файл
int lab2_write(int fd, const void *buf, size_t count, size_t *written) { return 0; }

// Перестановка позиции указателя на данные файла
off_t lab2_lseek(int fd, off_t offset, int whence) { return 0; }

// Синхронизация данных из кэша с диском
int lab2_fsync(int fd) { return 0; }
