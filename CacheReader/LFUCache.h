#pragma once

#include <utility>

#include "Global.h"

namespace os {
    using T = std::vector<char>;

    class LFUCache {
        struct Block {
            int key;
            int frequency;
            T value;

            Block(int k, T& v, int f) : key(k), value(std::move(v)), frequency(f) {}
        };
        struct Compare {
            bool operator()(const Block* a, const Block* b) const {
                if (a->frequency == b->frequency) return a->key < b->key;
                return a->frequency < b->frequency;
            }
        };

        size_t cacheSize;
        std::unordered_map<int, Block*> cache; // key -> block
        std::set<Block*, Compare> blocks; // (begin) least freq used -> most (end)

    public:
        explicit LFUCache(size_t cacheSize);

        void refer(int key, T& value);
        T& get(int key, const std::function<T&(int key)>& supplier);

    private:
        void evictLFU();
        void incrementFrequency(int key);
    };
}
