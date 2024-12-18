#include "CacheReader.h"

#define THROW_ERR(err) throw std::runtime_error("Error " err " with code " + std::to_string(GetLastError()))

namespace os {
    CacheReader::CacheReader(const char *path)
        : path(path) {
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

        if (hFile == INVALID_HANDLE_VALUE) {
            THROW_ERR("opening file");
        }
        isOpen = true;
    }

    void CacheReader::close() {
        if (!isOpen) return;
        fsync();
        if (!CloseHandle(hFile)) THROW_ERR("closing file");
        hFile = INVALID_HANDLE_VALUE;
        isOpen = false;
    }

    size_t CacheReader::read(void *buffer, size_t size) {
        DWORD bytes_read;
        if (!ReadFile(hFile, buffer, static_cast<DWORD>(size), &bytes_read, nullptr)) {
            close();
            THROW_ERR("reading file");
        }
        return bytes_read;
    }

    size_t CacheReader::write(const void *buffer, size_t size) {
        DWORD bytes_written;
        if (!WriteFile(hFile, buffer, static_cast<DWORD>(size), &bytes_written, nullptr)) {
            close();
            THROW_ERR("writing file");
        }
        return bytes_written;
    }

    off_t CacheReader::lseek(off_t offset, int whence) {
        DWORD new_pos = SetFilePointer(hFile, offset, nullptr, whence);
        if (new_pos == INVALID_SET_FILE_POINTER) {
            close();
            THROW_ERR("lseeking file");
        }
        return static_cast<off_t>(new_pos);
    }

    void CacheReader::fsync() {
    }
}
