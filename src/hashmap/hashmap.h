#pragma once

struct hashmap {
    int size;
    int bucket_size;
    struct hashmap_bucket* bucket_array;
};

struct hashmap_bucket {
    int hash_index;
    int array_size;
    struct hashmap_entry* array;
};

struct hashmap_entry {
    int key;
    int value;
};

struct hashmap* new_hashmap();
void free_hashmap(struct hashmap* map);

void hashmap_put(struct hashmap* map, int key, int value);
struct hashmap_entry* hashmap_get(const struct hashmap* map, int key);
void hashmap_remove(struct hashmap* map, int key);