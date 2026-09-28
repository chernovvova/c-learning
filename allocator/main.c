#include <stdio.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct Block {
    bool is_free;
    size_t size;
};

int main(void)
{
    void *ptr = malloc(100);

    printf("address: %p\n", ptr);
    printf("mod 16: %zu\n", (uintptr_t)ptr % 16);

    free(ptr);
}