#include "LFUCache.h"

namespace os {
    template<typename T>
    LFUCache<T>::LFUCache(size_t cacheSize) : cacheSize(cacheSize) {
    }

    template<typename T>
    void LFUCache<T>::refer(int key, const T& value) {
        if (cache.contains(key)) {
            incrementFrequency(key);
            return;
        }

        if (cache.size() >= cacheSize) {
            evictLFU();
        }
        auto *newBlock = new Block(key, value, 1);
        cache[key] = newBlock;
        blocks.insert(newBlock);
        std::cout << "Cache block " << key << " inserted.\n";
    }

    template<typename T>
    T& LFUCache<T>::get(int key, std::function<T&(int)> supplier) {
        T& value = cache.contains(key) ? cache.at(key) : supplier(key);
        refer(key, value);
        return value;
    }

    template<typename T>
    void LFUCache<T>::evictLFU() {
        if (!blocks.empty()) {
            Block* lfuBlock = *blocks.begin(); // Находим блок с наименьшей частотой
            blocks.erase(blocks.begin());
            cache.erase(lfuBlock->key);
            std::cout << "Cache block " << lfuBlock->key << " removed.\n";
            delete lfuBlock; // Освобождаем память
        }
    }

    template<typename T>
    void LFUCache<T>::incrementFrequency(int key) {
        Block *block = cache[key];
        blocks.erase(block); // Удаляем из set
        ++block->frequency; // Увеличиваем частоту
        blocks.insert(block); // Вставляем обратно
    }
}
