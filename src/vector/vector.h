#pragma once
#include <stdbool.h>

typedef int vector_type;

struct vector {
    int size;
    int capacity;
    bool is_wrapping;
    vector_type* head; // array holds capacity * sizeof(vector_type)
};

struct vector_type_nullable {
    bool is_null;
    vector_type data;
};

struct vector* vector_new();
void vector_free(struct vector* vector);

void vector_push(struct vector* vector, vector_type value);
struct vector_type_nullable vector_pop(struct vector* vector);

vector_type* vector_at(const struct vector* vector, int index);

struct vector* vector_wrap_array(int size, vector_type* head);
struct vector* vector_deep_copy_array(int size, const vector_type* head);
struct vector* vector_clone(const struct vector* vector);