#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashtable.h"

static char *rand_string() {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK.";
    char *str;

    size_t size = rand() % 3;
    str = malloc(size + 1);
    size_t n;
    for (n = 0; n < size; n++) {
        int key = rand() % (int) (sizeof(charset) - 1);
        str[n] = charset[key];
    }
    str[size] = '\0';

    return str;
}

int main() {
    size_t i;
    hashtable_ctx *ht = hashtable_new(7);

    srand(time(NULL));

    const char *key = "kkey";
    const char *value = "vvalue";
    void *result;

    // get not exist key
    result = hashtable_get(ht, key);
    printf("hashtable_get should be NULL: %s\n", NULL == result ? "pass" : "fail");

    // set and get
    hashtable_set(ht, key, (void *)value);
    result = hashtable_get(ht, key);
    printf("hashtable_get should be value: %s\n", 0 == strcmp(value, (const char *)result) ? "pass" : "fail");

    // delete and get
    hashtable_delete(ht, key);
    result = hashtable_get(ht, key);
    printf("hashtable_get should be NULL: %s\n", NULL == result ? "pass" : "fail");

    //  10 thousand test case
    for (i = 0; i < 10000; i++) {
        char *randomkey = rand_string();
        hashtable_set(ht, randomkey, NULL);
        free(randomkey);
    }
    printf("hashtable_ctx used %zu\n", ht->used);

    // it
    for (i = 0; i < ht->size; i++) {
        hashtable_entry *entry = ht->table[i];
        while (entry) {
            entry = entry->next;
        }
    }

    for (i = 0; i < 100000; i++) {
        char *randomkey = rand_string();
        hashtable_delete(ht, randomkey);
        free(randomkey);
    }

    printf("hashtable_ctx used %zu\n", ht->used);

    hashtable_destroy(ht);

    return 0;
}
