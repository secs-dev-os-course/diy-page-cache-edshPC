#pragma once

#include "CacheReader.h"
#include "Global.h"

#define CACHE_PAGE_SIZE (4 * 1 << 10) // 4 Kb
#define CACHE_SIZE (32 * 1 << 20) // 32 Mb

namespace os {
    using Page = std::vector<char>;

    class LFUCache {
        struct Block {
            int key;
            int frequency;
            Page value;
            bool modified = false;

            Block(int k, Page &v, int f) : key(k), value(std::move(v)), frequency(f) {
            }
        };

        struct Compare {
            bool operator()(const Block *a, const Block *b) const {
                if (a->frequency == b->frequency) return a->key < b->key;
                return a->frequency < b->frequency;
            }
        };

        size_t cacheSize;
        std::unordered_map<int, Block *> cache; // key -> block
        std::set<Block *, Compare> blocks; // (begin) least freq used -> most (end)
        CacheReader& reader;

    public:
        LFUCache(size_t cacheSize, CacheReader& reader);

        void put_page(int key, Page &value);
        Page &get(int key, bool modify = false);
        void syncronize();

    private:
        void evictLFU();
        void incrementFrequency(int key);
    };
}
