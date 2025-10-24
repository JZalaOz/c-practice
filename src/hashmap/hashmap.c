#include "hashmap.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// for every LOAD_FACTOR of entries, 1 bucket
#define MIN_LOAD_FACTOR 0.5
#define TARGET_LOAD_FACTOR 0.75
#define MAX_LOAD_FACTOR 2.0
#define MINIMUM_BUCKET_COUNT 4

struct hashmap* new_hashmap() {
    struct hashmap* map = malloc(sizeof(struct hashmap));
    map->size = 0;
    map->bucket_size = MINIMUM_BUCKET_COUNT;
    map->bucket_array = malloc(map->bucket_size * sizeof(struct hashmap_bucket));

    for (int i = 0; i < map->bucket_size; i++) {
        struct hashmap_bucket* bucket = &map->bucket_array[i];
        bucket->hash_index = i;
        bucket->array_size = 0;
        bucket->array = NULL;
    }

    return map;
}

void free_hashmap(struct hashmap* map) {
    for (int i = 0; i < map->bucket_size; i++) {
        free(map->bucket_array[i].array);
    }
    free(map->bucket_array);
    free(map);
}

static unsigned int get_hash_index(const struct hashmap* map, int key) {
    return (unsigned int) key % (unsigned int) map->bucket_size;
}

// The load factor is the average entries per bucket
static double get_load_factor(const struct hashmap* map) {
    const double a = map->size;
    const double b = map->bucket_size;

    if (a == 0.0 || b == 0.0) {
        return 0;
    }

    return a / b;
}

static void hashmap_bucket_push(struct hashmap_bucket* bucket, const struct hashmap_entry* entry) {
    if (bucket->array != NULL) {
        bucket->array = realloc(bucket->array, ++bucket->array_size * sizeof(struct hashmap_entry));
    } else {
        bucket->array_size++;
        bucket->array = malloc(sizeof(struct hashmap_entry));
    }

    bucket->array[bucket->array_size - 1] = *entry;
}

static void try_resizing(struct hashmap* map) {
    const double load_factor = get_load_factor(map);
    const int new_bucket_size = (int) ((double) map->size / TARGET_LOAD_FACTOR);
    if ((load_factor > MIN_LOAD_FACTOR && load_factor < MAX_LOAD_FACTOR)
        || map->bucket_size == new_bucket_size || (double) new_bucket_size <= MINIMUM_BUCKET_COUNT) {
        return;
    }

    const int old_bucket_array_size = map->bucket_size;
    struct hashmap_bucket* old_bucket_array = map->bucket_array;

    map->bucket_size = new_bucket_size;
    map->bucket_array = malloc(map->bucket_size * sizeof(struct hashmap_bucket));

    for (int i = 0; i < map->bucket_size; i++) {
        struct hashmap_bucket* bucket = &map->bucket_array[i];
        bucket->array_size = 0;
        bucket->hash_index = i;
        bucket->array = NULL;
    }

    for (int i = 0; i < old_bucket_array_size; i++) {
        const struct hashmap_bucket* old_bucket = &old_bucket_array[i];
        for (int j = 0; j < old_bucket->array_size; j++) {
            const struct hashmap_entry* entry = &old_bucket->array[j];
            struct hashmap_bucket* bucket = &map->bucket_array[get_hash_index(map, entry->key)];
            hashmap_bucket_push(bucket, entry);
        }

        free(old_bucket->array);
    }

    free(old_bucket_array);
}

void hashmap_put(struct hashmap* map, int key, int value) {
    struct hashmap_entry* previous_entry = hashmap_get(map, key);
    if (previous_entry != NULL) {
        previous_entry->value = value;
        return;
    }

    const unsigned int hash_index = get_hash_index(map, key);
    struct hashmap_bucket* bucket = &map->bucket_array[hash_index];

    hashmap_bucket_push(bucket, &(struct hashmap_entry) {key, value});
    map->size++;
    try_resizing(map);
}

struct hashmap_entry* hashmap_get(const struct hashmap* map, int key) {
    const unsigned int hash_index = get_hash_index(map, key);
    const struct hashmap_bucket* bucket = &map->bucket_array[hash_index];

    for (int i = 0; i < bucket->array_size; i++) {
        struct hashmap_entry* entry = &bucket->array[i];
        if (entry->key == key) {
            return entry; // If someone edits the key in this struct, it will break
        }
    }

    return NULL;
}

void hashmap_remove(struct hashmap* map, int key) {
    const unsigned int hash_index = get_hash_index(map, key);
    struct hashmap_bucket* bucket = &map->bucket_array[hash_index];

    int array_index = 0;
    bool did_find_key = false;
    struct hashmap_entry* array;

    if (bucket->array_size == 0) {
        return;
    }

    if (bucket->array_size == 1) {
        array = NULL;
    } else {
        array = malloc(sizeof(struct hashmap_entry) * (bucket->array_size - 1));
    }

    for (int i = 0; i < bucket->array_size; i++) {
        const struct hashmap_entry* entry = &bucket->array[i];

        if (entry->key == key) {
            did_find_key = true;
            continue;
        }

        if (array != NULL) {
            array[array_index] = *entry;
        }
        array_index++;
    }

    if (!did_find_key) {
        free(array);
    } else {
        free(bucket->array);
        map->size--;
        bucket->array_size--;
        bucket->array = array;
        try_resizing(map);
    }
}