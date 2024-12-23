#include <corecrt_io.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include <Windows.h>
#include <fcntl.h>

#include "CacheReader/CacheReaderAPI.h"

namespace os {
    const size_t FileSize = 64 * (1ULL << 20ULL) / sizeof(int64_t); // 256 Mb
    const int64_t ToFind = -777;
    const int32_t seed = 777;

    void prepeare() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<int64_t> dis(1, FileSize - 1);

        std::vector<int64_t> output(FileSize);

        for (size_t i = 0; i < FileSize; ++i) {
            output[i] = dis(gen);
        }
        const size_t place = dis(gen);
        output[place] = ToFind; // Найти это значение в случайном месте

        std::ofstream out_file("input.bin", std::ios::binary | std::ios::trunc); // NOLINT
        if (!out_file) {
            throw std::runtime_error("Failed to open output file");
        }
        out_file.write(reinterpret_cast<const char *>(output.data()), sizeof(int64_t) * FileSize); // NOLINT
        out_file.close();
        std::cout << "File size num " << FileSize << '\n';
        std::cout << "Value placed at " << place << '\n';
    }

    int compute() {
        int fd = -1;
        int err = lab2_open(&fd, "input.bin");
        if (err) {
            prepeare();
            err = lab2_open(&fd, "input.bin");
        }
        if (err) {
            throw std::runtime_error("Failed to open input file");
        }

        volatile int found = 0;

        std::mt19937_64 gen(seed);
        std::uniform_int_distribution<int> dis(0, FileSize - 1);

        while (found == 0) {
            int number_pos = dis(gen);
            lab2_lseek(fd, number_pos * sizeof(ToFind), SEEK_SET, nullptr);
            int64_t num = 0;
            if (lab2_read(fd, &num, sizeof(num), nullptr) != 0) {
                lab2_close(fd);
                throw std::runtime_error("Error reading from file");
            }
            if (num == ToFind) {
                found = number_pos;
            }
        }
        lab2_close(fd);
        std::cout << "found at " << found << '\n';
        return found;
    }

    int compute_no_cache() {
        int fd = -1;
        _sopen_s(&fd, "input.bin", _O_RDONLY | _O_BINARY,_SH_DENYNO,_S_IREAD);
        if (fd < 0) {
            prepeare();
            _sopen_s(&fd, "input.bin", _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD);
        }
        if (fd < 0) {
            throw std::runtime_error("Failed to open input file");
        }

        volatile int found = 0;

        std::mt19937_64 gen(seed);
        std::uniform_int_distribution<int> dis(0, FileSize - 1);

        while (found == 0) {
            int number_pos = dis(gen);
            _lseek(fd, number_pos * sizeof(ToFind), SEEK_SET);
            int64_t num = 0;
            if (_read(fd, &num, sizeof(num)) <= 0) {
                _close(fd);
                throw std::runtime_error("Error reading from file");
            }
            if (num == ToFind) {
                found = number_pos;
            }
        }
        _close(fd);
        std::cout << "found at " << found << '\n';
        return found;
    }

} // namespace
