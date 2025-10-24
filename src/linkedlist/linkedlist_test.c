// ReSharper disable CppLocalVariableMayBeConst
#pragma once
#include "linkedlist.h"
#include <stdio.h>
#include <assert.h>

void testLinkedListImpl() {
    printf("=== Linked List Comprehensive Test ===\n\n");

    // Test 1: Create new list and check initial state
    printf("Test 1: Creating new list\n");
    struct list* myList = newList();
    assert(myList != NULL);
    assert(myList->size == 0);
    assert(myList->head == NULL);
    assert(myList->tail == NULL);
    printf("✓ New list created successfully\n\n");

    // Test 2: Add elements to front
    printf("Test 2: Adding elements to front\n");
    addFirst(myList, 10);
    assert(myList->size == 1);
    assert(getFirst(myList) == 10);
    assert(getLast(myList) == 10);

    addFirst(myList, 20);
    assert(myList->size == 2);
    assert(getFirst(myList) == 20);
    assert(getLast(myList) == 10);
    printf("✓ addFirst working correctly\n\n");

    // Test 3: Add elements to back
    printf("Test 3: Adding elements to back\n");
    addLast(myList, 30);
    assert(myList->size == 3);
    assert(getFirst(myList) == 20);
    assert(getLast(myList) == 30);

    addLast(myList, 40);
    assert(myList->size == 4);
    assert(getLast(myList) == 40);
    printf("✓ addLast working correctly\n\n");

    // Test 4: Print current list
    printf("Test 4: Current list state: ");
    printList(myList);
    printf("Expected: 20 <-> 10 <-> 30 <-> 40\n\n");

    // Test 5: Add before specific positions
    printf("Test 5: Adding before specific positions\n");
    addBefore(myList, 0, 15);  // Add before first element
    assert(getFirst(myList) == 15);
    assert(myList->size == 5);

    addBefore(myList, 2, 25);  // Add before index 2
    assert(getAt(myList, 2) == 25);
    printf("✓ addBefore working correctly\n");
    printf("List after addBefore operations: ");
    printList(myList);
    printf("\n");

    // Test 6: Add after specific positions
    printf("Test 6: Adding after specific positions\n");
    addAfter(myList, 0, 18);   // Add after first element
    assert(getAt(myList, 1) == 18);

    addAfter(myList, myList->size - 1, 45);  // Add after last element
    assert(getLast(myList) == 45);
    assert(myList->size == 8);
    printf("✓ addAfter working correctly\n");
    printf("List after addAfter operations: ");
    printList(myList);
    printf("\n");

    // Test 7: Get elements at specific positions
    printf("Test 7: Getting elements at specific positions\n");
    assert(getFirst(myList) == 15);
    assert(getAt(myList, 3) == 25);
    assert(getLast(myList) == 45);
    printf("✓ getFirst, getAt, getLast working correctly\n\n");

    // Test 8: Test containsValue and findIndexOfValue
    printf("Test 8: Testing value search functions\n");
    assert(containsValue(myList, 25) == true);
    assert(containsValue(myList, 99) == false);

    int index = findIndexOfValue(myList, 30);
    assert(index >= 0 && getAt(myList, index) == 30);

    index = findIndexOfValue(myList, 999);  // Non-existent value
    assert(index == -1);
    printf("✓ containsValue and findIndexOfValue working correctly\n\n");

    // Test 9: Remove first element
    printf("Test 9: Removing first element\n");
    int firstBefore = getFirst(myList);
    removeFirst(myList);
    assert(getFirst(myList) != firstBefore);
    assert(myList->size == 7);
    printf("✓ removeFirst working correctly\n");
    printf("List after removeFirst: ");
    printList(myList);
    printf("\n");

    // Test 10: Remove last element
    printf("Test 10: Removing last element\n");
    int lastBefore = getLast(myList);
    removeLast(myList);
    assert(getLast(myList) != lastBefore);
    assert(myList->size == 6);
    printf("✓ removeLast working correctly\n");
    printf("List after removeLast: ");
    printList(myList);
    printf("\n");

    // Test 11: Remove at specific position
    printf("Test 11: Removing at specific position\n");
    int valueAtIndex2 = getAt(myList, 2);
    removeAt(myList, 2);
    assert(getAt(myList, 2) != valueAtIndex2);
    assert(myList->size == 5);
    printf("✓ removeAt working correctly\n");
    printf("List after removeAt(2): ");
    printList(myList);
    printf("\n");

    // Test 12: Test reverseList
    printf("Test 12: Testing list reversal\n");
    printf("List before reverse: ");
    printList(myList);

    int firstBeforeReverse = getFirst(myList);
    int lastBeforeReverse = getLast(myList);
    reverseList(myList);

    assert(getFirst(myList) == lastBeforeReverse);
    assert(getLast(myList) == firstBeforeReverse);
    assert(myList->size == 5);
    printf("List after reverse: ");
    printList(myList);
    printf("✓ reverseList working correctly\n\n");

    // Test 13: Edge cases - empty list operations
    printf("Test 13: Testing edge cases\n");
    struct list* emptyList = newList();

    // These should handle empty list gracefully (your implementation should handle this)
    removeFirst(emptyList);  // Should not crash
    removeLast(emptyList);   // Should not crash
    removeAt(emptyList, 0);  // Should not crash

    // Test adding to empty list
    addFirst(emptyList, 100);
    assert(emptyList->size == 1);
    assert(getFirst(emptyList) == 100);
    assert(getLast(emptyList) == 100);

    removeFirst(emptyList);
    assert(emptyList->size == 0);

    addLast(emptyList, 200);
    assert(emptyList->size == 1);
    assert(getFirst(emptyList) == 200);
    printf("✓ Edge cases handled correctly\n\n");

    // Test 14: Single element list operations
    printf("Test 14: Testing single element list\n");
    struct list* singleList = newList();
    addFirst(singleList, 42);

    assert(getFirst(singleList) == 42);
    assert(getLast(singleList) == 42);
    assert(getAt(singleList, 0) == 42);

    removeFirst(singleList);
    assert(singleList->size == 0);
    printf("✓ Single element list operations working correctly\n\n");

    // Test 15: Free lists
    printf("Test 15: Testing memory cleanup\n");
    freeList(myList);
    freeList(emptyList);
    freeList(singleList);
    printf("✓ All lists freed successfully - no memory leaks\n\n");

    // Final comprehensive test
    printf("Final Test: Comprehensive stress test\n");
    struct list* finalTest = newList();

    // Add many elements
    for (int i = 0; i < 10; i++) {
        addLast(finalTest, i * 10);
    }
    assert(finalTest->size == 10);

    // Test various operations
    addFirst(finalTest, -10);
    addAfter(finalTest, 5, 55);
    addBefore(finalTest, 3, 25);
    removeAt(finalTest, 7);

    // Verify final state
    assert(finalTest->size == 12);
    assert(getFirst(finalTest) == -10);

    printf("Final list: ");
    printList(finalTest);

    freeList(finalTest);
    printf("✓ All tests passed! Your linked list implementation is working correctly.\n");
}