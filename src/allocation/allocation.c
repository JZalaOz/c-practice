// NOLINTNEXTLINE
#define _GNU_SOURCE

#include "allocation.h"
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

static const struct page_type_entry* get_page_type_nearest_to_size(size_t size) {
    // TODO THIS
    // 0 - 4kb
    // 1 - 2mb
    // 2 - 1gb
    // 3 - 2gb
    // size - 512kb

    const struct page_type_entry* prev = &get_page_types()->array[0];

    for (int i = 1; i < get_page_types()->size; i++) {
        const struct page_type_entry* curr = &get_page_types()->array[i];

        if (size <= curr->byte_size) {
            // maybe Check if closer to curr then prev
            break;
        }

        prev = curr;
    }

    return prev;
}

static struct internal_allocation get_free_space_list_allocation() {
    static struct internal_allocation value;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        value.size = getpagesize();
        value.address = mmap(NULL, value.size,
            PROT_WRITE | PROT_READ,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
    }

    return value;
}

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

static bool is_valid_allocated_page_index(int index) {
    return !(index < 0 || index * sizeof(struct allocated_page) > get_allocated_pages().allocation.size);
}

static struct allocated_page* get_allocated_page_by_index(int index) {
    if (!is_valid_allocated_page_index(index))
        return NULL;
    return get_allocated_pages().allocation.address + (index * sizeof(struct allocated_page));
}

static struct free_space_entry* find_free_space_of_size(size_t size) {
    const struct allocated_page* curr = get_allocated_pages().head;
    struct free_space_entry* free_space_entry = NULL;

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

    return free_space_entry;
}

static struct free_space_entry* get_free_space_with_new_allocated_page(size_t size) {
    const struct page_type_entry* page_type = get_page_type_nearest_to_size(size + sizeof(struct user_allocation_meta));

    // Find allocated page entry that is not in use, or increase the size of the allocation for entries
    int allocated_page_index = 0;
    struct allocated_page* page;
    while (true) {
        page = get_allocated_page_by_index(allocated_page_index);

        if (page == NULL) {
            struct internal_allocation allocation = get_allocated_pages().allocation;
            const size_t new_size = allocation.size + getpagesize();
            allocation.address = mremap(allocation.address, allocation.size, new_size, MREMAP_MAYMOVE);
            allocation.size = new_size;

            allocated_page_index++;
            page = get_allocated_page_by_index(allocated_page_index);
            break;
        }

        if (!page->in_use)
            break;
        allocated_page_index++;
    }

    page->in_use = true;

    const size_t byte_count = page_type->byte_size * ((page_type->byte_size + size - 1) / size);
    int flags = MAP_PRIVATE | MAP_ANONYMOUS;
    if (!page_type->is_normal_page) {
        flags |= MAP_HUGETLB;
        flags |= page_type->flag;
    }

    page->address = mmap(NULL, byte_count,PROT_READ | PROT_WRITE, flags, -1, 0);
    page->size = byte_count;
    page->page_type = page_type;

    page->free_space_list_size = 1;
    page->free_space_list_head
}

void* my_malloc(size_t size) {
    if (size < 0)
        return NULL;

    struct free_space_entry* free_space_entry = find_free_space_of_size(size);
    if (free_space_entry == NULL) {
        free_space_entry = get_free_space_with_new_allocated_page(size);
    }
}

void* my_realloc(void* ptr, size_t size) {

}

void freedom(void* ptr) {

}