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

static int page_size() {
    static int value = -1;
    if (value == -1)
        value = getpagesize();
    return value;
}

struct huge_pages_entry {
    size_t byte_size;
    int flag;
};

struct huge_pages_map {
    int size;
    struct huge_pages_entry array[MAX_HUGE_PAGES];
};

static struct huge_pages_map huge_pages_map() {
    static struct huge_pages_map value;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        value.size = 0;
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
            if (byte_count == 0)
                continue;

            struct huge_pages_entry entry;
            entry.byte_size = byte_count;
            // hugetlb_encode.h says this is how its encoded
            entry.flag = (int) log2((double) byte_count);

            if (value.size >= MAX_HUGE_PAGES) {
                fprintf(stderr, "More huge pages in /sys/kernel/mm/hugepages than the max, which is %i", MAX_HUGE_PAGES);
                exit(1);
            }
            value.array[value.size++] = entry;
        }

        closedir(stream);
    }

    return value;
}

struct allocated_space {
    void* ptr;
    size_t size;
};

struct allocation_data {

};

static struct allocation_data allocation_data() {
    static struct allocation_data value;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;

    }
    return value;
}

void* my_malloc(size_t size) {
    page_size();
    huge_pages_map();
    //mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_ANONYMOUS, -1, 0);
}

void* my_realloc(void* ptr, size_t size) {

}

void freedom(void* ptr) {

}