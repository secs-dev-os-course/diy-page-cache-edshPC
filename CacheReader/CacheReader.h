#pragma once

#include "Global.h"

namespace os {
    class CacheReader {
        const char *path;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        bool isOpen = false;

    public:
        explicit CacheReader(const char *path);
        ~CacheReader();

        void open();
        void close();

        size_t read(void *buffer, size_t size);
        size_t write(const void *buffer, size_t size);
        off_t lseek(off_t offset, int whence);
        void fsync();

    };
}
