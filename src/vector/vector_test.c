#pragma once
#include "vector.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void testVectorBasic() {
    printf("=== Vector Basic Tests ===\n\n");

    // Test 1: Create new vector and check initial state
    printf("Test 1: Creating new vector\n");
    struct vector* vec = vector_new();
    assert(vec != NULL);
    assert(vec->size == 0);
    assert(vec->capacity == 8); // INITIAL_CAPACITY
    assert(vec->is_wrapping == false);
    assert(vec->head != NULL);
    printf("✓ New vector created successfully\n\n");

    // Test 2: Basic push and pop operations
    printf("Test 2: Basic push and pop operations\n");
    vector_push(vec, 10);
    vector_push(vec, 20);
    vector_push(vec, 30);

    assert(vec->size == 3);

    struct vector_type_nullable popped = vector_pop(vec);
    assert(popped.is_null == false);
    assert(popped.data == 30);
    assert(vec->size == 2);

    popped = vector_pop(vec);
    assert(popped.is_null == false);
    assert(popped.data == 20);
    assert(vec->size == 1);

    printf("✓ Basic push and pop working correctly\n\n");

    vector_free(vec);
}

void testVectorAt() {
    printf("Test 3: Testing vector_at function\n");
    struct vector* vec = vector_new();

    vector_push(vec, 100);
    vector_push(vec, 200);
    vector_push(vec, 300);

    vector_type* element = vector_at(vec, 0);
    assert(element != NULL);
    assert(*element == 100);

    element = vector_at(vec, 1);
    assert(element != NULL);
    assert(*element == 200);

    element = vector_at(vec, 2);
    assert(element != NULL);
    assert(*element == 300);

    // Test out of bounds
    element = vector_at(vec, 5);
    assert(element == NULL);

    element = vector_at(vec, -1);
    assert(element == NULL);

    printf("✓ vector_at working correctly\n\n");

    vector_free(vec);
}

void testVectorResize() {
    printf("Test 4: Testing automatic resizing\n");
    struct vector* vec = vector_new();

    // Fill beyond initial capacity
    for (int i = 0; i < 20; i++) {
        vector_push(vec, i * 10);
    }

    assert(vec->size == 20);
    assert(vec->capacity >= 20); // Should have grown

    // Verify all elements are correct
    for (int i = 0; i < 20; i++) {
        vector_type* element = vector_at(vec, i);
        assert(element != NULL);
        assert(*element == i * 10);
    }

    printf("✓ Vector resized correctly during push\n\n");

    // Test shrinking during pop
    printf("Test 5: Testing automatic shrinking\n");
    int initial_capacity = vec->capacity;

    // Pop most elements to trigger shrinking
    while (vec->size > 2) {
        vector_pop(vec);
    }

    // Capacity should have shrunk (but not below INITIAL_CAPACITY)
    assert(vec->capacity < initial_capacity);
    assert(vec->capacity >= 8); // INITIAL_CAPACITY

    printf("✓ Vector shrunk correctly during pop\n\n");

    vector_free(vec);
}

void testVectorPopEmpty() {
    printf("Test 6: Testing pop from empty vector\n");
    struct vector* vec = vector_new();

    struct vector_type_nullable popped = vector_pop(vec);
    assert(popped.is_null == true);

    // Push and pop to make empty again
    vector_push(vec, 42);
    popped = vector_pop(vec);
    assert(popped.is_null == false);
    assert(popped.data == 42);

    // Now pop from empty again
    popped = vector_pop(vec);
    assert(popped.is_null == true);

    printf("✓ Empty vector pop handled correctly\n\n");

    vector_free(vec);
}

void testVectorWrapArray() {
    printf("Test 7: Testing array wrapping\n");

    int array[] = {1, 2, 3, 4, 5};
    int array_size = 5;

    struct vector* wrapped_vec = vector_wrap_array(array_size, array);

    assert(wrapped_vec != NULL);
    assert(wrapped_vec->size == array_size);
    assert(wrapped_vec->capacity == array_size);
    assert(wrapped_vec->is_wrapping == true);
    assert(wrapped_vec->head == array); // Should point to the original array

    // Verify elements
    for (int i = 0; i < array_size; i++) {
        vector_type* element = vector_at(wrapped_vec, i);
        assert(element != NULL);
        assert(*element == array[i]);
        assert(element == &array[i]); // Should be same memory location
    }

    printf("✓ Array wrapping working correctly\n\n");

    // Note: Don't free the wrapped vector's head since we didn't allocate it
    free(wrapped_vec);
}

void testVectorDeepCopy() {
    printf("Test 8: Testing array deep copy\n");

    int original_array[] = {10, 20, 30, 40, 50};
    int array_size = 5;

    struct vector* copied_vec = vector_deep_copy_array(array_size, original_array);

    assert(copied_vec != NULL);
    assert(copied_vec->size == array_size);
    //assert(copied_vec->capacity == array_size);
    assert(copied_vec->is_wrapping == false);
    assert(copied_vec->head != original_array); // Should be different memory

    // Verify elements are copied correctly
    for (int i = 0; i < array_size; i++) {
        vector_type* element = vector_at(copied_vec, i);
        assert(element != NULL);
        assert(*element == original_array[i]);
    }

    // Modify original array and verify copied vector is unaffected
    original_array[0] = 999;
    vector_type* first_element = vector_at(copied_vec, 0);
    assert(*first_element == 10); // Should still be original value

    printf("✓ Deep copy working correctly\n\n");

    vector_free(copied_vec);
}

void testVectorClone() {
    printf("Test 9: Testing vector cloning\n");

    struct vector* original = vector_new();
    vector_push(original, 100);
    vector_push(original, 200);
    vector_push(original, 300);

    struct vector* clone = vector_clone(original);

    assert(clone != NULL);
    assert(clone->size == original->size);
    assert(clone->capacity == original->capacity);
    assert(clone->is_wrapping == false);
    assert(clone->head != original->head); // Different memory

    // Verify elements are the same
    for (int i = 0; i < original->size; i++) {
        vector_type* orig_element = vector_at(original, i);
        vector_type* clone_element = vector_at(clone, i);
        assert(orig_element != NULL);
        assert(clone_element != NULL);
        assert(*orig_element == *clone_element);
    }

    // Modify original and verify clone is unaffected
    vector_push(original, 400);
    assert(original->size == 4);
    assert(clone->size == 3); // Clone should be unchanged

    printf("✓ Vector cloning working correctly\n\n");

    vector_free(original);
    vector_free(clone);
}

void testVectorStress() {
    printf("Test 10: Stress test with many operations\n");
    struct vector* vec = vector_new();

    // Add many elements
    for (int i = 0; i < 1000; i++) {
        vector_push(vec, i);
    }
    assert(vec->size == 1000);

    // Verify all elements
    for (int i = 0; i < 1000; i++) {
        vector_type* element = vector_at(vec, i);
        assert(element != NULL);
        assert(*element == i);
    }

    // Remove some elements
    for (int i = 0; i < 500; i++) {
        struct vector_type_nullable popped = vector_pop(vec);
        assert(popped.is_null == false);
        assert(popped.data == 999 - i); // Elements are popped from end
    }
    assert(vec->size == 500);

    // Add more elements
    for (int i = 0; i < 250; i++) {
        vector_push(vec, i + 1000);
    }
    assert(vec->size == 750);

    printf("✓ Stress test passed\n\n");

    vector_free(vec);
}

void testVectorEdgeCases() {
    printf("Test 11: Testing edge cases\n");

    // Test with zero-sized array wrap
    printf("  Testing zero-sized array wrap\n");
    struct vector* empty_wrap = vector_wrap_array(0, NULL);
    assert(empty_wrap != NULL);
    assert(empty_wrap->size == 0);
    free(empty_wrap);

    // Test with zero-sized deep copy
    printf("  Testing zero-sized deep copy\n");
    struct vector* empty_copy = vector_deep_copy_array(0, NULL);
    assert(empty_copy != NULL);
    assert(empty_copy->size == 0);
    vector_free(empty_copy);

    // Test clone of empty vector
    printf("  Testing clone of empty vector\n");
    struct vector* empty_original = vector_new();
    struct vector* empty_clone = vector_clone(empty_original);
    assert(empty_clone != NULL);
    assert(empty_clone->size == 0);
    vector_free(empty_original);
    vector_free(empty_clone);

    printf("✓ Edge cases handled correctly\n\n");
}

void testVectorMemoryManagement() {
    printf("Test 12: Testing memory management\n");

    // Test normal vector free
    struct vector* normal_vec = vector_new();
    vector_push(normal_vec, 1);
    vector_push(normal_vec, 2);
    vector_free(normal_vec);

    // Test wrapped vector free (should not free the array)
    int external_array[] = {1, 2, 3};
    struct vector* wrapped_vec = vector_wrap_array(3, external_array);
    vector_free(wrapped_vec); // Should not crash

    printf("✓ Memory management working correctly\n\n");
}

void testVectorComprehensive() {
    printf("Test 13: Comprehensive final test\n");
    struct vector* vec = vector_new();

    // Mixed operations
    vector_push(vec, 1);
    vector_push(vec, 2);
    vector_push(vec, 3);

    assert(*vector_at(vec, 0) == 1);
    assert(*vector_at(vec, 1) == 2);
    assert(*vector_at(vec, 2) == 3);

    struct vector_type_nullable popped = vector_pop(vec);
    assert(popped.data == 3);
    assert(vec->size == 2);

    vector_push(vec, 4);
    vector_push(vec, 5);

    assert(vec->size == 4);
    assert(*vector_at(vec, 3) == 5);

    // Test all elements
    assert(*vector_at(vec, 0) == 1);
    assert(*vector_at(vec, 1) == 2);
    assert(*vector_at(vec, 2) == 4);
    assert(*vector_at(vec, 3) == 5);

    vector_free(vec);
    printf("✓ Comprehensive test passed\n\n");
}

void runAllVectorTests() {
    printf("=== Starting Vector Implementation Tests ===\n\n");

    testVectorBasic();
    testVectorAt();
    testVectorResize();
    testVectorPopEmpty();
    testVectorWrapArray();
    testVectorDeepCopy();
    testVectorClone();
    testVectorStress();
    testVectorEdgeCases();
    testVectorMemoryManagement();
    testVectorComprehensive();

    printf("🎉 All vector tests completed successfully!\n");
    printf("   Your vector implementation is working correctly.\n");
}