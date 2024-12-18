#include "CacheReader.h"

namespace os {
    CacheReader::CacheReader(const char *path)
        : path(path), fd(-1) {
    }

    CacheReader::~CacheReader() {
    }

    int CacheReader::open() {
    }

    void CacheReader::close() {
    }

    size_t CacheReader::read(void *buffer, size_t size) {
    }

    size_t CacheReader::write(const void *buffer, size_t size) {
    }

    off_t CacheReader::lseek(off_t offset, int whence) {
    }

    void CacheReader::fsync() {
    }
}
