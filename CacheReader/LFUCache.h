#pragma once

#include "Global.h"

namespace os {
    template<typename T>
    class LFUCache {
        struct Block {
            int key;
            int frequency;
            T value;

            Block(int k, const T& v, int f) : key(k), value(v), frequency(f) {}
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

        void refer(int key, const T& value);
        T& get(int key, std::function<T&(int key)> supplier);

    private:
        void evictLFU();
        void incrementFrequency(int key);
    };
}
