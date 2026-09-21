#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "allocator.h"
#include "test_allocator.h"

const size_t ALLOCATOR_MEMORY_SIZE = 4096;

struct Block {
    bool is_free;
    size_t size;
};

static unsigned char *memory;
static size_t offset;

enum AllocatorError allocator_init(void) {
    if (memory != NULL) {
        return ALLOCATOR_ALREADY_INITIALIZED;
    }

    memory = malloc(ALLOCATOR_MEMORY_SIZE);

    if (memory == NULL) {
        return ALLOCATOR_OUT_OF_MEMORY;
    }

    return ALLOCATOR_SUCCESS;
}

void split_block(struct Block* block, size_t size) {
    if (block->size - size <= sizeof(struct Block)) {
        return;
    }
    
    struct Block* splitted_block = (struct Block *)((unsigned char *) (block + 1) + size);
    splitted_block->is_free = true;
    splitted_block->size = block->size - size - sizeof(struct Block);
    block->size = size;
}

struct Block* find_free_block(size_t size) {
    size_t current_byte = 0;
   
    while (current_byte < offset) {
       struct Block *block = (struct Block*) &memory[current_byte];
       if (block->is_free && block->size >= size) {
           if (block->size - size > sizeof(struct Block)) {
               split_block(block, size);
           }
           return block;
       }
       current_byte += sizeof(struct Block) + block->size;
    }

    return NULL;
}

void *my_malloc(size_t size) {
    if (memory == NULL) {
        return NULL;
    }

    struct Block* block = find_free_block(size);

    if (block == NULL) {
        if (size > ALLOCATOR_MEMORY_SIZE - sizeof(struct Block) - offset) {
            return NULL;
        }
        block = (struct Block*) &memory[offset];
        block->size = size;
        offset += size + sizeof(struct Block);
    }
    block->is_free = false;
    
    unsigned char* data = (unsigned char*) (block + 1);
    
    return data;
}

struct Block *get_next_block(struct Block *block) {
    unsigned char * next_block = (unsigned char *) (block) + sizeof(struct Block) + block->size;
    if ((unsigned char *) &memory[offset] <= next_block) {
        return NULL;
    }
    return (struct Block *) next_block;
}

struct Block *get_previous_block(struct Block *block) {
    size_t current_byte = 0;
    struct Block *previous_block = NULL;

    while (&memory[current_byte] <= (unsigned char *) block) {
        struct Block *current_block = (struct Block *) &memory[current_byte];

        if (current_block == block) {
            return previous_block;
        }
        current_byte += sizeof(struct Block) + current_block->size;
        previous_block = current_block;
    }
    return NULL; 
}

void merge_blocks(struct Block *block, struct Block *next_block) {
    if ((unsigned char *)(block + 1) + block->size != (unsigned char *) next_block) {
        return;
    }
    block->size += next_block->size + sizeof(struct Block);
}

void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    struct Block *block = (struct Block*)ptr - 1;
    block->is_free = true;
    
    struct Block *next_block = get_next_block(block);
    if (next_block != NULL && next_block->is_free) {
        merge_blocks(block, next_block);
    }

    struct Block *previous_block = get_previous_block(block);
    if (previous_block != NULL && previous_block->is_free) {
        merge_blocks(previous_block, block);
    }
}

void allocator_destroy(void) {
    free(memory);
    memory = NULL;
    offset = 0;
}

void get_blocks_metadata() {
    size_t current_byte = 0;
   
    while (current_byte < offset) {
       struct Block *block = (struct Block*) &memory[current_byte];
       printf("Block:\n\taddress: %p,\n\tsize: %zu,\n\tis_free:%d\n", block, block->size, block->is_free);
       current_byte += sizeof(struct Block) + block->size;
    }
    printf("---------------------------\n");
}


// FOR TESTS
size_t get_offset_for_tests(void) {
    return offset;
}

unsigned char *get_memory_for_tests(void) {
    return memory;
}

size_t get_metadata_size(void) {
    return sizeof(struct Block);
}