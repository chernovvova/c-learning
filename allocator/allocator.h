#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

enum AllocatorError {
    ALLOCATOR_SUCCESS = 0,
    ALLOCATOR_ALREADY_INITIALIZED = 1,
    ALLOCATOR_OUT_OF_MEMORY = 2
};

enum AllocatorError allocator_init(void);

void *my_malloc(size_t size);
void my_free(void *ptr);
void allocator_destroy(void);
void get_blocks_metadata(void);

#endif