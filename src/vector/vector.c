#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

struct vector* vector_new() {
    struct vector* vector = malloc(sizeof(struct vector));
    vector->size = 0;
    vector->capacity = INITIAL_CAPACITY;
    vector->is_wrapping = false;
    vector->head = malloc(sizeof(vector_type) * INITIAL_CAPACITY);
    return vector;
}

void vector_free(struct vector* vector) {
    if (!vector->is_wrapping) {
        free(vector->head);
    }
    free(vector);
}

void vector_push(struct vector* vector, vector_type value) {
    if (vector->size >= vector->capacity) {
        const int new_capacity = vector->capacity * 2;
        vector_type* new_ptr = realloc(vector->head, sizeof(vector_type) * new_capacity);
        if (new_ptr == NULL) {
            printf("failed to realloc");
            exit(1);
        }
        vector->head = new_ptr;
        vector->capacity = new_capacity;
    }

    const int index = vector->size;
    vector->head[index] = value;
    vector->size++;
}

struct vector_type_nullable vector_pop(struct vector* vector) {
    if (vector->size == 0) {
        return (struct vector_type_nullable) {true};
    }

    const int index = vector->size - 1;
    struct vector_type_nullable return_data;
    return_data.is_null = false;
    return_data.data = vector->head[index];

    if (vector->size <= vector->capacity / 4) {
        const int new_capacity = vector->capacity / 2;
        if (new_capacity >= INITIAL_CAPACITY) {
            vector_type* new_ptr = realloc(vector->head, sizeof(vector_type) * new_capacity);
            if (new_ptr != NULL) {
                vector->head = new_ptr;
                vector->capacity = new_capacity;
            }
        }
    }

    vector->size--;
    return return_data;
}

vector_type* vector_at(const struct vector* vector, int index) {
    if (index < 0 || index >= vector->size) {
        return NULL;
    }

    return &vector->head[index];
}

struct vector* vector_wrap_array(int size, vector_type* head) {
    struct vector* vector = malloc(sizeof(struct vector));
    vector->size = size;
    vector->capacity = size;
    vector->is_wrapping = true;
    vector->head = head;
    return vector;
}

struct vector* vector_deep_copy_array(int size, const vector_type* head) {
    struct vector* vector = malloc(sizeof(struct vector));
    vector->size = size;
    vector->capacity = size;
    if (vector->capacity < INITIAL_CAPACITY) {
        vector->capacity = INITIAL_CAPACITY;
    }
    vector->is_wrapping = false;
    vector->head = malloc(sizeof(vector_type) * vector->capacity);
    memcpy(vector->head, head, sizeof(vector_type) * size);
    return vector;
}

struct vector* vector_clone(const struct vector* vector) {
    return vector_deep_copy_array(vector->size, vector->head);
}