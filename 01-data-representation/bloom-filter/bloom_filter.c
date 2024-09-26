#include "bloom_filter.h"
#include <stdlib.h>
#include <string.h>

#define SEED 29

uint64_t calc_hash(const char* str, uint64_t modulus, uint64_t seed) {
    uint64_t hash = 0;
    for (const char* ch = str; *ch != '\0'; ++ch) {
        uint64_t code = *ch - 'a';
        hash = (hash * seed + code) % modulus;
    }
    return hash;
}

void bloom_init(struct BloomFilter* bloom_filter, uint64_t set_size, hash_fn_t hash_fn, uint64_t hash_fn_count) {
    bloom_filter->set_size = ((set_size >> 6) + (set_size & 63) != 0) << 6;
    bloom_filter->set = (uint64_t*) malloc((bloom_filter->set_size >> 6) * sizeof(uint64_t));
    memset(bloom_filter->set, 0, (bloom_filter->set_size >> 6) * sizeof(*bloom_filter->set));
    bloom_filter->hash_fn = hash_fn;
    bloom_filter->hash_fn_count = hash_fn_count;
}

void bloom_destroy(struct BloomFilter* bloom_filter) {
    free(bloom_filter->set);
    bloom_filter->set = NULL;
}

void bloom_insert(struct BloomFilter* bloom_filter, Key key) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, SEED);
    bloom_filter->set[hash >> 6] |= (1ull << (hash & 63));
}

bool bloom_check(struct BloomFilter* bloom_filter, Key key) {
    uint64_t hash = bloom_filter->hash_fn(key, bloom_filter->set_size, SEED);
    return bloom_filter->set[hash >> 6] & (1ull << (hash & 63));
}
