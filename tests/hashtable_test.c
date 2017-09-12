#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "hashtable.h"
#include "minunit.h"

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

MU_TEST(hashtable_new_and_destroy) {
    hashtable_ctx *ht = hashtable_new(6);
    mu_check(0 == ht->used);
    hashtable_destroy(ht);
}

MU_TEST(hashtable_new_size) {
    hashtable_ctx *ht = hashtable_new(6);
    mu_check(11 == ht->size);
    hashtable_destroy(ht);

    ht = hashtable_new(1700);
    mu_check(1741 == ht->size);
    hashtable_destroy(ht);
}

MU_TEST(hashtable_set_get) {
    hashtable_ctx *ht = hashtable_new(6);

    void *value;
    bool success;

    value = hashtable_get(ht, "key");
    mu_check(NULL == value);

    success = hashtable_set(ht, "key", (void *)5);
    mu_check(true == success);

    value = hashtable_get(ht, "key");
    mu_check((void *)5 == value);

    hashtable_destroy(ht);
}

MU_TEST(hashtable_delete_test) {
    hashtable_ctx *ht = hashtable_new(6);

    void *value;
    bool success;

    success = hashtable_delete(ht, "key");
    mu_check(false == success);

    success = hashtable_set(ht, "key", (void *)5);
    mu_check(true == success);

    success = hashtable_delete(ht, "key");
    mu_check(true == success);

    value = hashtable_get(ht, "key");
    mu_check(NULL == value);

    hashtable_destroy(ht);
}

MU_TEST(hashtable_set_get_delete_random) {
    hashtable_ctx *ht = hashtable_new(100);

    int i;

    // random set
    for (i = 0; i < 10000; i++) {
        char *randomkey = rand_string();
        hashtable_set(ht, randomkey, NULL);
        free(randomkey);
    }

    // random get
    for (i = 0; i < 10000; i++) {
        char *randomkey = rand_string();
        hashtable_get(ht, randomkey);
        free(randomkey);
    }

    // random delete
    for (i = 0; i < 10000; i++) {
        char *randomkey = rand_string();
        hashtable_delete(ht, randomkey);
        free(randomkey);
    }

    hashtable_destroy(ht);
}

int main() {
    MU_RUN_TEST(hashtable_new_and_destroy);
    MU_RUN_TEST(hashtable_new_size);
    MU_RUN_TEST(hashtable_set_get);
    MU_RUN_TEST(hashtable_delete_test);
    MU_RUN_TEST(hashtable_set_get_delete_random);

    MU_REPORT();

    return 0;
}
