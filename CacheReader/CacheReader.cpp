#include "CacheReader.h"

#define THROW_ERR(err) throw std::runtime_error("Error " err " with code " + std::to_string(GetLastError()))

namespace os {
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
            OPEN_EXISTING, // Открыть файл, если он существует
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
        size_t bytes_written = 0;
        int start_page = offset / CACHE_PAGE_SIZE;
        int end_page = (offset + size - 1) / CACHE_PAGE_SIZE;

        for (int n_page = start_page; n_page <= end_page; ++n_page) {
            // get page from cache or read
            Page &page = cache.get(n_page, true);

            size_t page_offset = (n_page == start_page) ? (offset % CACHE_PAGE_SIZE) : 0;
            size_t bytes_to_copy = min(size - bytes_written, CACHE_PAGE_SIZE - page_offset);
            if (bytes_to_copy <= 0) break;
            if (page.size() < bytes_to_copy + page_offset) page.resize(bytes_to_copy + page_offset);

            std::copy_n(static_cast<const char *>(buffer) + bytes_written,
                        bytes_to_copy,
                        page.begin() + page_offset);

            bytes_written += bytes_to_copy;
            if (bytes_written >= size) break;
        }

        offset += bytes_written;
        return bytes_written;
    }

    off_t CacheReader::lseek(off_t off, int whence) {
        switch (whence) {
            case SEEK_SET:
                offset = off;
                break;
            case SEEK_CUR:
                offset += off;
                break;
            case SEEK_END:
                offset = mSetFilePointer(0, FILE_END);
                offset += off;
                break;
            default:
                throw std::runtime_error("Invalid lseek operation");
        }
        return static_cast<off_t>(offset);
    }

    void CacheReader::fsync() {
        cache.syncronize();
    }

    size_t CacheReader::readPage(int key, Page &page) {
        mSetFilePointer(key * CACHE_PAGE_SIZE);
        DWORD bytes_read;
        mReadFile(page.data(), page.size(), &bytes_read);
        return bytes_read;
    }

    void CacheReader::writePage(int key, Page &page) {
        size_t size = page.size();
        if (size == 0) return;
        int64_t actual = mSetFilePointer(key * CACHE_PAGE_SIZE);

        // если нужно записать страницу за пределами существующих
        std::vector<char> zero_data(key * CACHE_PAGE_SIZE - actual, 0);
        mWriteFile(zero_data.data(), zero_data.size());

        // выравнивание страницы по размеру сектора
        if (size < CACHE_PAGE_SIZE) {
            page.resize(CACHE_PAGE_SIZE, 0);
        }
        mWriteFile(page.data(), page.size());
        if (size < CACHE_PAGE_SIZE) {
            LARGE_INTEGER EOF_pos;
            EOF_pos.QuadPart = actual + size;
            // установка логического конца файла
            SetFileInformationByHandle(hFile, FileEndOfFileInfo, &EOF_pos, sizeof(EOF_pos));
            page.resize(size);
        }
    }

    void CacheReader::mReadFile(char *buf, size_t size, LPDWORD read) {
        if (!ReadFile(hFile, buf, size, read, nullptr))
            THROW_ERR("reading file");
    }

    void CacheReader::mWriteFile(const char *buf, size_t size, LPDWORD write) {
        if (!WriteFile(hFile, buf, size, write, nullptr))
            THROW_ERR("writing file");
    }

    int64_t CacheReader::mSetFilePointer(int64_t offset, int whence) {
        LARGE_INTEGER li, res;
        li.QuadPart = offset;
        if (!SetFilePointerEx(hFile, li, &res, whence))
            THROW_ERR("setting file pointer");
        return res.QuadPart;
    }
}
