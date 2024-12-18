#include "LFUCache.h"

namespace os {
    LFUCache::LFUCache(size_t cacheSize) : cacheSize(cacheSize) {
    }

    void LFUCache::refer(int key, T& value) {
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

    T& LFUCache::get(int key, const std::function<T&(int)>& supplier) {
        T& value = cache.contains(key) ? cache.at(key)->value : supplier(key);
        refer(key, value);
        return value;
    }

    void LFUCache::evictLFU() {
        if (!blocks.empty()) {
            Block* lfuBlock = *blocks.begin(); // Находим блок с наименьшей частотой
            blocks.erase(blocks.begin());
            cache.erase(lfuBlock->key);
            std::cout << "Cache block " << lfuBlock->key << " removed.\n";
            delete lfuBlock; // Освобождаем память
        }
    }
    
    void LFUCache::incrementFrequency(int key) {
        Block *block = cache[key];
        blocks.erase(block); // Удаляем из set
        ++block->frequency; // Увеличиваем частоту
        blocks.insert(block); // Вставляем обратно
    }
}
