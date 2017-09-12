#ifndef __HASHTABLE_H
#define __HASHTABLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct __hashtable_entry {
    struct __hashtable_entry *next;
    void *value;
    char key[0];
} hashtable_entry;

typedef struct {
    size_t used;
    size_t size;
    hashtable_entry **table;
} hashtable_ctx;

hashtable_ctx *hashtable_new(size_t size);

void hashtable_destroy(hashtable_ctx *ctx);

// return true if success, otherwise return false
bool hashtable_set(hashtable_ctx *ctx, const char *key, void *value);

// return the value if success, otherwise return NULL
void *hashtable_get(hashtable_ctx *ctx, const char *key);

// return true if success, otherwise return false
bool hashtable_delete(hashtable_ctx *ctx, const char *key);

// return true if success, otherwise return false
bool hashtable_resize(hashtable_ctx *ctx);

// return true if success, otherwise return false
bool hashtable_expand(hashtable_ctx *ctx, size_t size);

#endif
