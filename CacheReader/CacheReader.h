#pragma once

#include "Global.h"

namespace os {
    class CacheReader {
        const char *path;
        int fd;

    public:
        CacheReader(const char *path);
        ~CacheReader();

        int open();
        void close();

        size_t read(void *buffer, size_t size);
        size_t write(const void *buffer, size_t size);
        off_t lseek(off_t offset, int whence);
        void fsync();

    };
}
