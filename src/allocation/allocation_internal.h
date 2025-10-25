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
    bool is_normal_page;
    size_t byte_size;
    int flag;
};

// Sorted by lowest to highest byte_size
struct page_type_list {
    int size;
    struct page_type_entry array[MAX_PAGES];
};

// Each allocated page will have a list of free spaces, so it will be easy to put new allocations
struct allocated_page; // Because it's defined in here before the struct is defined, and allocated_page uses this
struct free_space_entry {
    bool in_use;
    void* address;
    size_t size;
    struct allocated_page* allocated_page;

    struct free_space_entry* next;
    struct free_space_entry* prev;
};

struct allocated_page {
    bool in_use;
    void* address;
    size_t size;
    const struct page_type_entry* page_type;

    // The list will be sorted where head is the lowest size, and tail is the highest
    int free_space_list_size;
    struct free_space_entry* free_space_list_head;

    struct allocated_page* next;
    struct allocated_page* prev;
};

struct allocated_pages_list {
    struct allocated_page* head;
    struct internal_allocation allocation;
};

// This is allocated just before the user allocation
struct user_allocation_meta {
    int allocated_page_index;
    size_t size; // does not include the size of this meta struct
    void* address; // after this meta struct
};