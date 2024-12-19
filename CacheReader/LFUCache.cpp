#include "LFUCache.h"

namespace os {
    LFUCache::LFUCache(size_t cacheSize, CacheReader &reader)
        : cacheSize(cacheSize), reader(reader) {
    }

    void LFUCache::put_page(int key, Page &value) {
        if (cache.size() >= cacheSize) {
            evictLFU();
        }
        auto *newBlock = new Block(key, value, 1);
        cache[key] = newBlock;
        blocks.insert(newBlock);
        std::cout << "Cache block " << key << " inserted.\n";
    }

    Page &LFUCache::get(int key, bool modify) {
        if (cache.contains(key)) {
            incrementFrequency(key);
        } else {
            Page new_page(CACHE_PAGE_SIZE);
            size_t read = reader.readPage(key, new_page);
            new_page.resize(read);
            put_page(key, new_page);
        }
        if (modify) cache.at(key)->modified = true;
        return cache.at(key)->value;
    }

    void LFUCache::syncronize() {
        for (auto block : blocks) {
            if (block->modified) {
                reader.writePage(block->key, block->value);
                block->modified = false;
            }
        }
    }

    void LFUCache::evictLFU() {
        if (!blocks.empty()) {
            Block *lfuBlock = *blocks.begin(); // Находим блок с наименьшей частотой
            blocks.erase(blocks.begin());
            cache.erase(lfuBlock->key);
            std::cout << "Cache block " << lfuBlock->key << " removed.\n";
            if (lfuBlock->modified)
                reader.writePage(lfuBlock->key, lfuBlock->value); // сливаем на диск
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
