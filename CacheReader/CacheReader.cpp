#include "CacheReader.h"

#define THROW_ERR(err) throw std::runtime_error("Error " err " with code " + std::to_string(GetLastError()))

namespace os {
    //static const size_t CACHE_PAGE_SIZE = 4 * 1 << 10;
    //static const size_t CACHE_SIZE = 32 * 1 << 20;

    CacheReader::CacheReader(const char *path)
        : cache(CACHE_SIZE / CACHE_PAGE_SIZE, this),
          path(path) {
    }

    CacheReader::~CacheReader() { close(); }

    void CacheReader::open() {
        hFile = CreateFile(
            path, // Имя файла
            GENERIC_READ | GENERIC_WRITE, // Открыть для чтения и записи
            FILE_SHARE_READ | FILE_SHARE_WRITE, // Разрешить совместный доступ на чтение и запись
            nullptr, // Без безопасности
            OPEN_ALWAYS, // Открыть файл, если он существует, или создать новый
            FILE_FLAG_NO_BUFFERING, // Отключить кэширование
            nullptr // Без шаблона
        );

        if (hFile == INVALID_HANDLE_VALUE)
            THROW_ERR("opening file");

        isOpen = true;
    }

    void CacheReader::close() {
        if (!isOpen) return;
        fsync();
        if (!CloseHandle(hFile))
            THROW_ERR("closing file");
        hFile = INVALID_HANDLE_VALUE;
        isOpen = false;
    }

    size_t CacheReader::read(void *buffer, size_t size) {
        size_t bytes_read = 0;
        int start_page = offset / CACHE_PAGE_SIZE;
        int end_page = (offset + size - 1) / CACHE_PAGE_SIZE;

        for (int n_page = start_page; n_page <= end_page; ++n_page) {
            // get page from cache or read
            Page &page = cache.get(n_page);

            size_t page_offset = (n_page == start_page) ? (offset % CACHE_PAGE_SIZE) : 0;
            size_t bytes_to_copy = min(size - bytes_read, page.size() - page_offset);
            if (bytes_to_copy <= 0) break;

            std::copy_n(page.begin() + page_offset,
                        bytes_to_copy,
                        static_cast<char *>(buffer) + bytes_read);

            bytes_read += bytes_to_copy;
            if (bytes_read >= size) break;
        }

        offset += bytes_read;
        return bytes_read;
    }

    size_t CacheReader::write(const void *buffer, size_t size) {
        DWORD bytes_written;
        if (!WriteFile(hFile, buffer, static_cast<DWORD>(size), &bytes_written, nullptr)) {
            THROW_ERR("writing file");
        }
        return bytes_written;
    }

    off_t CacheReader::lseek(off_t off, int whence) {
        offset = SetFilePointer(hFile, off, nullptr, whence);
        return offset;
    }

    void CacheReader::fsync() {
        cache.syncronize();
    }

    size_t CacheReader::readPage(int key, Page &page) {
        SetFilePointer(hFile, key * CACHE_PAGE_SIZE, nullptr, FILE_BEGIN);
        DWORD bytes_read;
        if (!ReadFile(hFile, page.data(), page.size(), &bytes_read, nullptr))
            THROW_ERR("reading file");
        return bytes_read;
    }

    void CacheReader::writePage(int key, const Page &page) {
        off_t actual = SetFilePointer(hFile, key * CACHE_PAGE_SIZE, nullptr, FILE_BEGIN);
        std::vector<char> zero_data(key * CACHE_PAGE_SIZE - actual, 0); // if can't reach page offset in file
        if (!WriteFile(hFile, zero_data.data(), zero_data.size(), nullptr, nullptr))
            THROW_ERR("writing file");
        if (!WriteFile(hFile, page.data(), page.size(), nullptr, nullptr))
            THROW_ERR("writing file");
    }
}
