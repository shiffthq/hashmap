#include <stdio.h>
#include <stdint.h>

#define MURMURHASH_SEED 5381

// https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
static uint32_t MurmurHash2 (const void *key, size_t len) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    #define m 0x5bd1e995
    #define r 24

    // Initialize the hash to a 'random' value
    uint32_t h = MURMURHASH_SEED ^ len;

    // Mix 4 bytes at a time into the hash
    const unsigned char * data = (const unsigned char *)key;

    while (len >= 4) {
        uint32_t k = *(uint32_t *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch(len) {
    case 3:
        h ^= data[2] << 16;
        /* FALLTHROUGH */
    case 2:
        h ^= data[1] << 8;
        /* FALLTHROUGH */
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.
    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
