#include "allocator.h"
#include "test_allocator.h"
#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void test_allocator_init() {
    enum AllocatorError init_result = allocator_init();
    
    assert(get_memory_for_tests() != NULL);
    assert(get_offset_for_tests() == 0);
    assert(init_result == ALLOCATOR_SUCCESS);
    
    allocator_destroy();
}

void test_allocator_destroy() {
    allocator_init();
    my_malloc(100);

    allocator_destroy();

    assert(get_memory_for_tests() == NULL);
    assert(get_offset_for_tests() == 0);
}

void test_uninitialized_allocator_malloc() {
    assert(my_malloc(100) == NULL);
}

void test_malloc_new_block_creation() {
    allocator_init();
    
    unsigned char *data = my_malloc(100);

    assert(data == get_memory_for_tests() + get_metadata_size());

    allocator_destroy();
}

void test_malloc_with_free_block() {
    allocator_init();

    unsigned char *first_block = my_malloc(100);
    my_free(first_block);
    unsigned char *second_block = my_malloc(50);
   
    assert(second_block == first_block);
    
    allocator_destroy();
}

void test_malloc_with_insufficient_size_free_for_split_block() {
    allocator_init();

    size_t first_block_size = 10 * alignof(max_align_t);
    unsigned char *first_block = my_malloc(first_block_size); 
    my_free(first_block);
    unsigned char *second_block = my_malloc(first_block_size - get_metadata_size());

    assert(second_block == first_block); 
        
    allocator_destroy();
}

void test_malloc_split_free_block() {
    allocator_init();

    size_t first_block_size = 10 * alignof(max_align_t);
    unsigned char *block = my_malloc(first_block_size);
    my_free(block);
    size_t splitted_block_size = (size_t) first_block_size / 2 - get_metadata_size();
    unsigned char *first_splitted_block = my_malloc(splitted_block_size);
    unsigned char *second_splitted_block = my_malloc(splitted_block_size);

    assert(first_splitted_block == block);
    assert(second_splitted_block == block + get_metadata_size() + splitted_block_size);
        
    allocator_destroy();
}

void test_malloc_zero_size() {
    allocator_init();

    assert(my_malloc(0) == NULL);
    assert(get_offset_for_tests() == 0);

    allocator_destroy();
}

void test_free_with_null() {
    my_free(NULL);
}

void test_free_with_next_block() {
    allocator_init();
    
    size_t block_size = 10 * alignof(max_align_t);
    size_t next_block_size = 10 * alignof(max_align_t);
    unsigned char *block = my_malloc(block_size);
    unsigned char *next_block = my_malloc(next_block_size);
    my_free(next_block);
   
    my_free(block);
    unsigned char *new_block = my_malloc(block_size + next_block_size + get_metadata_size());

    assert(new_block == block);
    
    allocator_destroy();
}

void test_free_with_previous_block() {
    allocator_init();

    size_t previous_block_size = 10 * alignof(max_align_t);
    size_t block_size = 10 * alignof(max_align_t);
    unsigned char *previous_block = my_malloc(previous_block_size);
    unsigned char *block = my_malloc(block_size);
    my_free(previous_block);
    
    my_free(block);
    unsigned char *new_block = my_malloc(previous_block_size + block_size + get_metadata_size());
    
    assert(new_block == previous_block);
        
    allocator_destroy();
}

void test_free_with_next_and_previous_block() {
    allocator_init();

    size_t previous_block_size = 10 * alignof(max_align_t);
    size_t block_size = 10 * alignof(max_align_t);
    size_t next_block_size = 10 * alignof(max_align_t);
    unsigned char *previous_block = my_malloc(previous_block_size);
    unsigned char *block = my_malloc(block_size);
    unsigned char *next_block = my_malloc(next_block_size);
    my_free(previous_block);
    my_free(next_block);

    my_free(block);
    unsigned char *new_block = my_malloc(previous_block_size + next_block_size + block_size + 2 * get_metadata_size());

    assert(new_block == previous_block);
    
    allocator_destroy();
}

void test_alignment() {
    allocator_init();
    
    size_t first_block_size = 1;
    size_t second_block_size = 11;
    unsigned char *first_block = my_malloc(first_block_size);
    unsigned char *second_block = my_malloc(second_block_size);
   
    assert((uintptr_t)first_block % alignof(max_align_t) == 0);
    assert((uintptr_t)second_block % alignof(max_align_t) == 0);
    assert(second_block == first_block + alignof(max_align_t) + get_metadata_size());

    allocator_destroy();
}

void run_all_tests() {
    test_allocator_init();
    test_allocator_destroy();
    test_uninitialized_allocator_malloc();
    test_malloc_new_block_creation();
    test_malloc_with_free_block();
    test_malloc_with_insufficient_size_free_for_split_block();
    test_malloc_split_free_block();
    test_malloc_zero_size();
    test_free_with_null();
    test_free_with_next_block();
    test_free_with_previous_block();
    test_free_with_next_and_previous_block();
    test_alignment();
}

int main() {
    run_all_tests();
    printf("All tests passed!");
    return 0;
}