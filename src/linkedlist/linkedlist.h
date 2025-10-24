#pragma once

#include <stdbool.h>

struct listEntry {
    int data;
    struct listEntry* prev;
    struct listEntry* next;
};

struct list {
    int size;
    struct listEntry* head;
    struct listEntry* tail;
};

struct list* newList();

void addFirst(struct list* list, int data);
void addBefore(struct list* list, int index, int data);
void addAfter(struct list* list, int index, int data);
void addLast(struct list* list, int data);

void removeFirst(struct list* list);
void removeAt(struct list* list, int index);
void removeLast(struct list* list);

int getFirst(const struct list* list);
int getAt(const struct list* list, int index);
int getLast(const struct list* list);

int findIndexOfValue(const struct list* list, int data);
bool containsValue(const struct list* list, int data);
void reverseList(struct list* list);

void freeList(struct list* list);
void printList(const struct list* list);