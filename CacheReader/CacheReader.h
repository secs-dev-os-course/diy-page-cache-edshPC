#pragma once

#include "Global.h"
#include "LFUCache.h"

namespace os {
    class CacheReader {
        LFUCache cache;
        const char *path;
        HANDLE hFile = INVALID_HANDLE_VALUE;
        bool isOpen = false;
        int64_t offset = 0;

    public:
        explicit CacheReader(const char *path);
        ~CacheReader();

        void open();
        void close();

        size_t read(void *buffer, size_t size);
        size_t write(const void *buffer, size_t size);
        off_t lseek(off_t off, int whence);
        void fsync();

        size_t readPage(int key, Page& page);
        void writePage(int key, Page &page);

    private:
        void mReadFile(char *buf, size_t size, LPDWORD read = nullptr);
        void mWriteFile(const char *buf, size_t size, LPDWORD write = nullptr);
        int64_t mSetFilePointer(int64_t offset, int whence = FILE_BEGIN);
    };
}
