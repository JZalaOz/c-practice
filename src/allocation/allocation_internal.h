#pragma once
#include <stdbool.h>

#define MAX_PAGES 10

// In each page, whenever a space is allocated
// There will be
//  - always 1 free space decreases in size, to make room for it
//  - always 1 user allocation added, for the allocation
//  - might be 1 free space added, to fill in the gap that was previously taken by the free space changed above
// For deallocating, it is similar
//  - always 1 free space increases in size, to make up for the space taken by the previous allocation
//  - always 1 user allocation removed, for the deallocation
//  - might be 1 free space removed, if its space can be taken by the previously increased sized free space

// Internal allocations
struct internal_allocation {
    void* address;
    size_t size;
};

// Page type size (including default/normal page size)
struct page_type_entry {
    size_t byte_size;
    int flag;
    bool is_normal_page;
};

// Sorted by lowest to highest byte_size
struct page_type_list {
    struct page_type_entry array[MAX_PAGES];
    int size;
};

struct page_metadata {
    size_t size;
    const struct page_type_entry* page_type;
    struct chunk_metadata* head_chunk;
    struct chunk_metadata* tail_chunk;
};

struct chunk_metadata {
    struct page_metadata* page;
    size_t size;

    // Per page
    struct chunk_metadata* prev;
    struct chunk_metadata* next;

    // Across pages
    struct chunk_metadata* prev_free;
    struct chunk_metadata* next_free;

    bool is_free;
};