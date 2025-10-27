// NOLINTNEXTLINE
#define _GNU_SOURCE
#include "allocation.h"

#include <assert.h>

#include "allocation_internal.h"
#include <sys/mman.h>
#include <linux/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int qsort_compare_page_type_entry(const void* a, const void* b) {
    const int l = (int) ((struct page_type_entry*) a)->byte_size;
    const int r = (int) ((struct page_type_entry*) b)->byte_size;
    return l - r;
}

static const struct page_type_list* get_page_types() {
    static struct page_type_list value;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;

        value.size = 1;
        value.array[0].is_normal_page = true;
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
            entry.is_normal_page = false;
            entry.byte_size = byte_count;
            // hugetlb_encode.h says this is how its encoded
            entry.flag = (int) log2((double) byte_count);

            if (value.size >= MAX_PAGES) {
                fprintf(stderr, "More huge pages in /sys/kernel/mm/hugepages than the max, which is %i\n", MAX_PAGES);
                exit(1);
            }
            value.array[value.size++] = entry;
        }

        qsort(value.array, value.size, sizeof(struct page_type_entry), qsort_compare_page_type_entry);
        closedir(stream);
    }

    return &value;
}

// Finds the biggest page size with the least extra space
static const struct page_type_entry* get_page_type_best_for_size(size_t size) {
    const struct page_type_entry* prev = &get_page_types()->array[0];

    for (int i = 1; i < get_page_types()->size; i++) {
        const struct page_type_entry* curr = &get_page_types()->array[i];

        if (size <= curr->byte_size) {
            break;
        }

        prev = curr;
    }

    return prev;
}

static struct page_metadata* head_page = NULL;
static struct page_metadata* tail_page = NULL;

static struct chunk_metadata* find_free_chunk_of_size(size_t size) {
    const struct chunk_metadata* chunk = head_page->head_chunk;
    if (!chunk->is_free) {
        chunk = chunk->next_free;
    }

    while (chunk != NULL) {
        chunk = chunk->next_free;
    }

    while (curr != NULL && free_space_entry == NULL) {
        struct free_space_entry* entry = curr->free_space_list_head;

        while (entry != NULL) {
            if (entry->size < size) {
                entry = entry->next;
                continue;
            }

            free_space_entry = entry;
            break;
        }

        curr = curr->next;
    }

    return chunk;
}

static struct page_metadata* get_new_page(size_t size) {
    const struct page_type_entry* page_type = get_page_type_best_for_size(size + sizeof(struct page_metadata) + sizeof(struct chunk_metadata) * 2);

    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (!page_type->is_normal_page) {
        flags |= MAP_HUGETLB;
        flags |= page_type->flag;
    }

    const size_t byte_count = page_type->byte_size / ((page_type->byte_size + size - 1) / size);
    const void* address = mmap(NULL, byte_count, PROT_READ | PROT_WRITE, flags, -1, 0);
    struct page_metadata* page = (struct page_metadata*) address;

    page->size = byte_count;
    page->page_type = page_type;

    struct chunk_metadata* chunk = (struct chunk_metadata*) (address + sizeof(struct page_metadata));
    page->head_chunk = chunk;
    page->tail_chunk = chunk;

    chunk->page = page;
    chunk->size = byte_count - sizeof(struct page_metadata);
    chunk->prev = NULL;
    chunk->next = NULL;

    if (tail_page != NULL) {
        if (tail_page->tail_chunk->is_free) {
            chunk->prev_free = tail_page->tail_chunk;
        } else {
            chunk->prev_free = tail_page->tail_chunk->prev_free;
        }
    }

    chunk->next_free = NULL;

    if (head_page == NULL) {
        head_page = page;
        tail_page = page;
    } else {
        assert(tail_page->tail_chunk == NULL);
        tail_page->tail_chunk = chunk;
    }
}

void* my_malloc(size_t size) {
    if (size < 0)
        return NULL;

    struct free_space_entry* free_space_entry = find_free_space_of_size(size);
    if (free_space_entry == NULL) {
        free_space_entry = get_free_space_with_new_page(size);
    }
}

void* my_realloc(void* ptr, size_t size) {

}

void freedom(void* ptr) {

}