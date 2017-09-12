#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "murmur2.c"

#define HASHTABLE_EXPAND_THROTTLE 70

// https://gcc.gnu.org/onlinedocs/gcc-4.7.1/libstdc%2B%2B/api/a01194_source.html
const static size_t prime_numbers[] = {
    5,
    11,
    23,
    47,
    97,
    199,
    409,
    823,
    1741,
    3469,
    6949,
    14033,
    28411,
    57557,
    116731,
    236897,
    480881,
    976369,
    1982627,
    4026031,
    8175383,
    16601593,
    33712729,
    68460391,
    139022417,
    282312799,
    573292817,
    1164186217,
    2364114217,
    4294967291
};
const static int prime_number_count = sizeof(prime_numbers) / sizeof(prime_numbers[0]);

static size_t get_prev_prime(size_t size) {
    int i;

    for (i = 1; i < prime_number_count; i++) {
        if (prime_numbers[i] >= size) {
            return prime_numbers[i - 1];
        }
    }

    return prime_numbers[0];
}

static size_t get_next_prime(size_t size) {
    int i;

    for (i = 0; i < prime_number_count; i++) {
        if (prime_numbers[i] >= size) {
            return prime_numbers[i];
        }
    }

    return prime_numbers[i - 1];
}

static size_t hashtable_hash(hashtable_ctx *ctx, const char *key) {
    return MurmurHash2(key, strlen(key)) % ctx->size;
}

static hashtable_entry *hashtable_new_entry(const char *key, uint32_t keyLen, void *value) {
    hashtable_entry *entry = (hashtable_entry *)malloc(sizeof(hashtable_entry) + keyLen);
    if (NULL == entry) {
        return NULL;
    }

    memcpy(entry->key, key, keyLen);
    entry->value = value;
    entry->next = NULL;
    return entry;
}

static bool hashtable_need_expand(hashtable_ctx *ctx) {
    // cannot expand anymore
    if (ctx->size >= prime_numbers[prime_number_count - 1]) {
        return false;
    }

    // used / size > 70%
    if ((ctx->used * 100 / ctx->size) > HASHTABLE_EXPAND_THROTTLE) {
        return true;
    }

    return false;
}

hashtable_ctx *hashtable_new(size_t size) {
    hashtable_ctx *ctx = calloc(1, sizeof(hashtable_ctx));
    if (NULL == ctx) {
        return NULL;
    }

    size = get_next_prime(size);
    ctx->size = size;
    ctx->used = 0;

    ctx->table = calloc(ctx->size, sizeof(hashtable_entry *));
    if (NULL == ctx->table) {
        free(ctx);
        return NULL;
    }

    return ctx;
}

void hashtable_destroy(hashtable_ctx *ctx) {
    size_t i;
    hashtable_entry *current;
    hashtable_entry *next;

    for (i = 0; i < ctx->size; i++) {
        current = ctx->table[i];
        while (current) {
            next = current->next;
            free(current);
            current = next;
        }
    }
    free(ctx->table);
    free(ctx);
}

bool hashtable_set(hashtable_ctx *ctx, const char *key, void *value) {
    uint32_t index = hashtable_hash(ctx, key);

    hashtable_entry *current = ctx->table[index];

    while (current) {
        if (0 == strcmp(current->key, key)) {
            current->value = value;
            return true;
        }
        current = current->next;
    }

    hashtable_entry *newItem = hashtable_new_entry(key, strlen(key) + 1, value);
    if (NULL == newItem) {
        return false;
    }

    ctx->used++;
    newItem->next = ctx->table[index];
    ctx->table[index] = newItem;

    if (hashtable_need_expand(ctx)) {
        hashtable_expand(ctx, get_next_prime(ctx->size + 1));
    }

    return true;
}

void *hashtable_get(hashtable_ctx *ctx, const char *key) {
    uint32_t index = hashtable_hash(ctx, key);

    hashtable_entry *current = ctx->table[index];

    while (current) {
        if (0 == strcmp(current->key, key)) {
            return current->value;
        }
        current = current->next;
    }

    return NULL;
}

bool hashtable_delete(hashtable_ctx *ctx, const char *key) {
    uint32_t index = hashtable_hash(ctx, key);

    hashtable_entry *current = ctx->table[index];
    hashtable_entry *prev;

    if (NULL == current) {
        return false;
    } else if (0 == strcmp(current->key, key)) {
        ctx->table[index] = current->next;
        free(current);
        ctx->used--;
        return true;
    } else {
        prev = current;
        current = prev->next;

        while (current) {
            if (0 == strcmp(current->key, key)) {
                prev->next = current->next;
                free(current);
                ctx->used--;
                return true;
            }
            prev = current;
            current = current->next;
        }
    }

    return false;
}

bool hashtable_expand(hashtable_ctx *ctx, size_t size) {
    size = get_next_prime(size);

    if (ctx->size == size) {
        return false;
    }

    if ((ctx->used * 100 / size) > HASHTABLE_EXPAND_THROTTLE) {
        return false;
    }

    hashtable_ctx *tmpCtx = hashtable_new(size);
    if (NULL == tmpCtx) {
        return false;
    }

    hashtable_entry *current;
    hashtable_entry *next;
    size_t i;

    for (i = 0; i < ctx->size; i++) {
        current = ctx->table[i];
        while (current) {
            next = current->next;
            if (false == hashtable_set(tmpCtx, current->key, current->value)) {
                hashtable_destroy(tmpCtx);
                return false;
            }
            current = next;
        }
    }

    for (i = 0; i < ctx->size; i++) {
        current = ctx->table[i];
        while (current) {
            next = current->next;
            free(current);
            current = next;
        }
    }
    free(ctx->table);

    ctx->size = size;
    ctx->table = tmpCtx->table;
    free(tmpCtx);

    return true;
}

bool hashtable_resize(hashtable_ctx *ctx) {
    // should not overflow
    if ((ctx->used < 1) < ctx->used) {
        return false;
    }

    // ensure used/size < 50%
    size_t size = get_next_prime(ctx->used < 1);
    if (size >= ctx->size) {
        return false;
    }

    return hashtable_expand(ctx, size);
}
