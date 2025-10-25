#include "allocation.h"

#include <sys/mman.h>
#include <linux/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HUGE_PAGES 10

// Internal allocations
struct internal_allocation {
    void* address;
    size_t size;
};

struct page_type_entry {
    size_t byte_size;
    int flag;
};

// Sorted by lowest to highest byte_size
struct page_type_list {
    int size;
    struct page_type_entry array[MAX_HUGE_PAGES];
};

static int qsort_compare_page_type_entry(const void* a, const void* b) {
    const int l = (int) ((struct page_type_entry*) a)->byte_size;
    const int r = (int) ((struct page_type_entry*) b)->byte_size;
    return l - r;
}

static struct page_type_list get_page_types() {
    static struct page_type_list value;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;

        value.size = 1;
        value.array[0].byte_size = getpagesize();
        value.array[0].flag = (int) log2(getpagesize());

        DIR* stream = opendir("/sys/kernel/mm/hugepages");
        if (stream == NULL) {
            fprintf(stderr, "allocation: Cannot find folder /sys/kernel/mm/hugepages\n");
            exit(1);
        }

        while (true) {
            // hugepages-<size>kB, where <size> is the page size
            const int prefix_size = 10;

            const struct dirent* dir = readdir(stream);
            if (dir == NULL)
                break;

            const size_t byte_count = strtol(dir->d_name + prefix_size, NULL, 10) << 10;
            if (byte_count == 0 || byte_count == getpagesize())
                continue;

            struct page_type_entry entry;
            entry.byte_size = byte_count;
            // hugetlb_encode.h says this is how its encoded
            entry.flag = (int) log2((double) byte_count);

            if (value.size >= MAX_HUGE_PAGES) {
                fprintf(stderr, "More huge pages in /sys/kernel/mm/hugepages than the max, which is %i\n", MAX_HUGE_PAGES);
                exit(1);
            }
            value.array[value.size++] = entry;
        }

        qsort(value.array, value.size, sizeof(struct page_type_entry), qsort_compare_page_type_entry);
        closedir(stream);
    }

    return value;
}

struct allocated_page_free_space_entry {
    void* address;
    size_t size;
    struct allocated_page_free_space_entry* next;
    struct allocated_page_free_space_entry* prev;
};

// In each page, whenever a new user allocation is added
// There is always 1 free space changed, 1 free space added, and 1 user space added
// When removing a user allocation 1 free space removed, 1 free space changed, 1 user space removed
struct allocated_page {
    void* address;
    size_t size;
    int huge_page_flag;
    struct allocated_page_free_space_entry* free_space_list;
    struct allocated_page* next;
    struct allocated_page* prev;
};

struct user_allocation {
    void* address;
    size_t size;
    struct allocated_page* page;
};

struct allocated_pages_list {
    struct allocated_page* head;
    struct internal_allocation allocation;
};

struct user_allocations_list {
    struct user_allocation* head;
    struct internal_allocation allocation;
};

static struct allocated_pages_list get_allocated_pages() {
    static struct allocated_pages_list value;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        value.allocation.size = getpagesize();
        value.allocation.address = mmap(NULL, value.allocation.size,
            PROT_WRITE | PROT_READ,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
    }
    return value;
}

// get_user_allocations

static void initialize() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        get_page_types();
        get_allocated_pages();
    }
}

void* my_malloc(size_t size) {
    initialize();
    //mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void* my_realloc(void* ptr, size_t size) {

}

void freedom(void* ptr) {

}