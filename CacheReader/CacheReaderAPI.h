#pragma once

#ifdef CACHE_READER_EXPORTS
#define CRAPI __declspec(dllexport)
#else
#define CRAPI __declspec(dllimport)
#endif

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif


// Открытие файла по заданному пути файла, доступного для чтения
CRAPI int lab2_open(const char *path);

// Закрытие файла по хэндлу
CRAPI int lab2_close(int fd);

// Чтение данных из файла
CRAPI int lab2_read(int fd, void *buf, size_t count, size_t *read);

// Запись данных в файл
CRAPI int lab2_write(int fd, const void *buf, size_t count, size_t *written);

// Перестановка позиции указателя на данные файла
CRAPI off_t lab2_lseek(int fd, off_t offset, int whence);

// Синхронизация данных из кэша с диском
CRAPI int lab2_fsync(int fd);


#ifdef __cplusplus
}
#endif
