#ifndef ALLOCATOR_TEST_H
#define ALLOCATOR_TEST_H

#include <stddef.h>

size_t get_offset_for_tests(void);
unsigned char *get_memory_for_tests(void);
size_t get_metadata_size(void);

#endif