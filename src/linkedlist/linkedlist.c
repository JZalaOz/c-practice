#include "linkedlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct list* newList() {
    struct list* list = malloc(sizeof(struct list));
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

static struct listEntry* newListEntry(int data) {
    // NOLINTNEXTLINE
    struct listEntry* entry = malloc(sizeof(struct listEntry));
    entry->data = data;
    entry->next = NULL;
    entry->prev = NULL;
    return entry;
}

static bool isValidIndex(const struct list* list, int index) {
    if (index < 0 || index >= list->size) {
        return false;
    }

    return true;
}

static struct listEntry* getNodeAt(const struct list* list, int index) {
    if (!isValidIndex(list, index) || list->size == 0) {
        printf("error huh");
        exit(EXIT_FAILURE);
    }

    struct listEntry* entry = list->head;
    int entryIndex = 0;

    while (entryIndex != index && entry != NULL) {
        entry = entry->next;
        entryIndex++;
    }

    return entry;
}

void addFirst(struct list* list, int data) {
    struct listEntry* entry = newListEntry(data);
    entry->next = list->head;
    if (list->head != NULL) {
        list->head->prev = entry;
    }
    list->head = entry;
    if (list->tail == NULL) {
        list->tail = list->head;
    }
    list->size++;
}

void addBefore(struct list* list, int index, int data) {
    if (!isValidIndex(list, index)) {
        return;
    }

    // if size is 0, head and tail are both null
    if (list->size == 0) {
        addFirst(list, data);
        return;
    }

    struct listEntry* nodeAt = getNodeAt(list, index);
    struct listEntry* entry = newListEntry(data);

    if (nodeAt->prev == NULL) { // if prev is null, means this head
        list->head = entry;
        entry->next = nodeAt;
        nodeAt->prev = entry;
    } else {
        entry->next = nodeAt;
        entry->prev = nodeAt->prev;
        nodeAt->prev->next = entry;
        nodeAt->prev = entry;
    }

    list->size++;
}

void addAfter(struct list* list, int index, int data) {
    if (!isValidIndex(list, index)) {
        return;
    }

    // if size is 0, head and tail are both null
    if (list->size == 0) {
        addFirst(list, data);
        return;
    }

    // if you want to add after tail
    if (index == list->size - 1) {
        struct listEntry* entry = newListEntry(data);

        list->tail->next = entry;
        entry->prev = list->tail;
        list->tail = entry;
        list->size++;
        return;
    }

    addBefore(list, index + 1, data);
}

void addLast(struct list* list, int data) {
    struct listEntry* entry = newListEntry(data);
    entry->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = entry;
    }
    list->tail = entry;
    if (list->head == NULL) {
        list->head = list->tail;
    }
    list->size++;
}

void removeFirst(struct list* list) {
    if (list->head == NULL) {
        return;
    }

    struct listEntry* head = list->head;
    list->head = list->head->next;

    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    free(head);
    list->size--;
}

void removeAt(struct list* list, const int index) {
    if (!isValidIndex(list, index) || list->size == 0) {
        return;
    }

    struct listEntry* node = getNodeAt(list, index);

    if (node->prev == NULL && node->next == NULL) {
        list->head = NULL;
        list->tail = NULL;
    } else if (node->prev == NULL) { // if prev is null, it's the head
        list->head = node->next;
        list->head->prev = NULL;
    } else if (node->next == NULL) { // if next is null, it's the tail
        list->tail = node->prev;
        list->tail->next = NULL;
    } else  {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    list->size--;
    free(node);
}

void removeLast(struct list* list) {
    if (list->tail == NULL) {
        return;
    }

    struct listEntry* tail = list->tail;
    list->tail = list->tail->prev;

    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    free(tail);
    list->size--;
}

int getFirst(const struct list* list) {
    return list->head->data;
}

int getAt(const struct list* list, int index) {
    if (!isValidIndex(list, index) || list->size == 0) {
        return -1;
    }

    const struct listEntry* entry = getNodeAt(list, index);
    return entry->data;
}

int getLast(const struct list* list) {
    return list->tail->data;
}

int findIndexOfValue(const struct list* list, int data) {
    int index = 0;

    while (index < list->size) {
        const int value = getAt(list, index);
        if (value == data) {
            return index;
        }
        index++;
    }

    return -1;
}

bool containsValue(const struct list* list, int data) {
    return findIndexOfValue(list, data) != -1;
}

void reverseList(struct list* list) {
    if (list->size < 2) {
        return;
    }

    struct listEntry* temp;
    struct listEntry* curr = list->head;

    while (curr != NULL) {
        temp = curr->next;
        curr->next = curr->prev;
        curr->prev = temp;
        curr = temp;
    }

    temp = list->head;
    list->head = list->tail;
    list->tail = temp;
}

void freeList(struct list* list) {
    struct listEntry* entry = list->head;

    while (entry != NULL) {
        struct listEntry* temp = entry;
        entry = entry->next;
        free(temp);
    }

    free(list);
}

void printList(const struct list* list) {
    printf("Printing list of size %i:\n", list->size);
    const struct listEntry* entry = list->head;
    while (entry != NULL) {
        printf("%i", entry->data);
        entry = entry->next;
        if (entry != NULL) {
            printf(" -> ");
        }
    }
    printf("\nDone printing\n");
}