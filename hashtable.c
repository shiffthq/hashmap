#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "murmur2.c"

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

hashtable_ctx *hashtable_new(size_t size) {
    hashtable_ctx *ctx = calloc(1, sizeof(hashtable_ctx));
    if (NULL == ctx) {
        return NULL;
    }

    ctx->size = size;
    ctx->used = 0;

    ctx->table = calloc((size_t)ctx->size, sizeof(hashtable_entry *));
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
