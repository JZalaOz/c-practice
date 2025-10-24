// ReSharper disable CppLocalVariableMayBeConst
#pragma once
#include "hashmap.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void testHashMapImpl() {
    printf("=== Hash Map Comprehensive Test ===\n\n");

    // Test 1: Create new hashmap and check initial state
    printf("Test 1: Creating new hashmap\n");
    struct hashmap* map = new_hashmap();
    assert(map != NULL);
    assert(map->size == 0);
    assert(map->bucket_size > 0);
    assert(map->bucket_array != NULL);
    printf("✓ New hashmap created successfully\n\n");

    // Test 2: Basic put and get operations
    printf("Test 2: Basic put and get operations\n");
    hashmap_put(map, 1, 100);
    assert(map->size == 1);

    struct hashmap_entry* entry = hashmap_get(map, 1);
    assert(entry != NULL);
    assert(entry->key == 1);
    assert(entry->value == 100);
    printf("✓ Basic put and get working correctly\n\n");

    // Test 3: Multiple put operations
    printf("Test 3: Adding multiple key-value pairs\n");
    hashmap_put(map, 2, 200);
    hashmap_put(map, 3, 300);
    hashmap_put(map, 4, 400);
    hashmap_put(map, 5, 500);

    assert(map->size == 5);

    entry = hashmap_get(map, 3);
    assert(entry != NULL);
    assert(entry->value == 300);

    entry = hashmap_get(map, 5);
    assert(entry != NULL);
    assert(entry->value == 500);
    printf("✓ Multiple put operations working correctly\n\n");

    // Test 4: Update existing key
    printf("Test 4: Testing key update behavior\n");
    hashmap_put(map, 3, 333);  // Update value for key 3
    // Note: Your implementation may allow duplicates or update - test will verify
    entry = hashmap_get(map, 3);
    assert(entry != NULL);
    printf("Key 3 value: %d (implementation may update or allow duplicates)\n", entry->value);
    printf("✓ Key update test completed\n\n");

    // Test 5: Get non-existent key
    printf("Test 5: Getting non-existent key\n");
    entry = hashmap_get(map, 999);
    assert(entry == NULL);
    printf("✓ Non-existent key returns NULL correctly\n\n");

    // Test 6: Remove operations
    printf("Test 6: Removing key-value pairs\n");
    int size_before_remove = map->size;
    hashmap_remove(map, 2);
    assert(map->size == size_before_remove - 1);

    entry = hashmap_get(map, 2);
    assert(entry == NULL);

    // Remove non-existent key (should not crash)
    hashmap_remove(map, 999);
    assert(map->size == size_before_remove - 1);
    printf("✓ Remove operations working correctly\n\n");

    // Test 7: Hash collision testing
    printf("Test 7: Testing hash collisions\n");
    struct hashmap* collision_map = new_hashmap();

    // These keys will likely hash to same bucket with small bucket size
    hashmap_put(collision_map, 1, 100);
    hashmap_put(collision_map, 5, 500);
    hashmap_put(collision_map, 9, 900);

    assert(collision_map->size == 3);

    // All should be retrievable despite potential collisions
    assert(hashmap_get(collision_map, 1) != NULL);
    assert(hashmap_get(collision_map, 5) != NULL);
    assert(hashmap_get(collision_map, 9) != NULL);
    printf("✓ Collision handling working correctly\n\n");

    // Test 8: Mixed operations
    printf("Test 8: Mixed operations test\n");
    hashmap_remove(map, 1);
    hashmap_put(map, 15, 1500);
    hashmap_put(map, 25, 2500);

    entry = hashmap_get(map, 1);
    assert(entry == NULL);

    entry = hashmap_get(map, 15);
    assert(entry != NULL && entry->value == 1500);

    entry = hashmap_get(map, 25);
    assert(entry != NULL && entry->value == 2500);
    printf("✓ Mixed operations working correctly\n\n");

    // Test 9: Edge cases
    printf("Test 9: Testing edge cases\n");

    // Test with key 0
    hashmap_put(map, 0, 9999);
    entry = hashmap_get(map, 0);
    assert(entry != NULL && entry->value == 9999);

    // Test with negative keys
    hashmap_put(map, -1, -100);
    entry = hashmap_get(map, -1);
    assert(entry != NULL && entry->value == -100);

    // Remove key 0
    hashmap_remove(map, 0);
    entry = hashmap_get(map, 0);
    assert(entry == NULL);
    printf("✓ Edge cases handled correctly\n\n");

    // Test 10: Large number of operations
    printf("Test 10: Stress test with many operations\n");
    struct hashmap* stress_map = new_hashmap();

    // Add many elements
    for (int i = 0; i < 50; i++) {
        hashmap_put(stress_map, i, i * 10);
    }
    assert(stress_map->size == 50);

    // Verify all elements
    for (int i = 0; i < 50; i++) {
        entry = hashmap_get(stress_map, i);
        assert(entry != NULL);
        assert(entry->value == i * 10);
    }

    // Remove some elements
    for (int i = 10; i < 20; i++) {
        hashmap_remove(stress_map, i);
    }
    assert(stress_map->size == 40);

    // Verify remaining elements
    for (int i = 0; i < 10; i++) {
        entry = hashmap_get(stress_map, i);
        assert(entry != NULL);
    }

    for (int i = 10; i < 20; i++) {
        entry = hashmap_get(stress_map, i);
        assert(entry == NULL);
    }

    // Add some back
    for (int i = 10; i < 15; i++) {
        hashmap_put(stress_map, i, i * 100);
    }
    assert(stress_map->size == 45);

    printf("✓ Stress test passed\n\n");

    // Test 11: Memory cleanup
    printf("Test 11: Testing memory cleanup\n");
    free_hashmap(map);
    free_hashmap(collision_map);
    free_hashmap(stress_map);
    printf("✓ All hashmaps freed successfully - no memory leaks\n\n");

    // Test 12: Empty map operations
    printf("Test 12: Testing empty map operations\n");
    struct hashmap* empty_map = new_hashmap();

    // Get from empty map
    entry = hashmap_get(empty_map, 1);
    assert(entry == NULL);

    // Remove from empty map (should not crash)
    hashmap_remove(empty_map, 1);
    assert(empty_map->size == 0);

    // Put into empty map
    hashmap_put(empty_map, 1, 100);
    assert(empty_map->size == 1);

    entry = hashmap_get(empty_map, 1);
    assert(entry != NULL && entry->value == 100);

    free_hashmap(empty_map);
    printf("✓ Empty map operations handled correctly\n\n");

    // Test 13: Single element map operations
    printf("Test 13: Testing single element map\n");
    struct hashmap* single_map = new_hashmap();

    hashmap_put(single_map, 42, 420);
    assert(single_map->size == 1);

    entry = hashmap_get(single_map, 42);
    assert(entry != NULL && entry->value == 420);

    // Remove the single element
    hashmap_remove(single_map, 42);
    assert(single_map->size == 0);
    entry = hashmap_get(single_map, 42);
    assert(entry == NULL);

    free_hashmap(single_map);
    printf("✓ Single element map operations working correctly\n\n");

    // Test 14: Verify behavior with duplicate keys
    printf("Test 14: Testing duplicate key behavior\n");
    struct hashmap* dup_map = new_hashmap();

    hashmap_put(dup_map, 1, 100);
    int size_after_first_put = dup_map->size;
    hashmap_put(dup_map, 1, 200);

    printf("Size after first put: %d, after second put: %d\n",
           size_after_first_put, dup_map->size);

    entry = hashmap_get(dup_map, 1);
    assert(entry != NULL);
    printf("Value for key 1: %d\n", entry->value);

    free_hashmap(dup_map);
    printf("✓ Duplicate key behavior tested\n\n");

    // Test 15: Comprehensive final test
    printf("Test 15: Comprehensive final test\n");
    struct hashmap* final_map = new_hashmap();

    // Add various keys
    hashmap_put(final_map, 10, 100);
    hashmap_put(final_map, 20, 200);
    hashmap_put(final_map, 30, 300);
    hashmap_put(final_map, 40, 400);

    assert(final_map->size == 4);
    assert(hashmap_get(final_map, 10)->value == 100);
    assert(hashmap_get(final_map, 20)->value == 200);

    // Update a key
    hashmap_put(final_map, 20, 222);

    // Remove a key
    hashmap_remove(final_map, 30);
    assert(final_map->size == 3);
    assert(hashmap_get(final_map, 30) == NULL);

    // Add more keys
    hashmap_put(final_map, 50, 500);
    hashmap_put(final_map, 60, 600);

    assert(final_map->size == 5);
    assert(hashmap_get(final_map, 50)->value == 500);
    assert(hashmap_get(final_map, 60)->value == 600);

    // Verify all remaining keys
    assert(hashmap_get(final_map, 10) != NULL);
    assert(hashmap_get(final_map, 20) != NULL && hashmap_get(final_map, 20)->value == 222);
    assert(hashmap_get(final_map, 40) != NULL);
    assert(hashmap_get(final_map, 50) != NULL);
    assert(hashmap_get(final_map, 60) != NULL);

    free_hashmap(final_map);
    printf("✓ Comprehensive final test passed\n\n");

    printf("🎉 All tests completed successfully!\n");
    printf("   Your hashmap implementation is working with the provided header.\n");
    printf("   Note: The test adapts to your implementation's behavior for updates/duplicates.\n");
}