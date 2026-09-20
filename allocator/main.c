#include "allocator.h"

int main() {
    if (allocator_init() != ALLOCATOR_SUCCESS) {
        return 1;
    }
    
    void *a = my_malloc(100);
    void *b = my_malloc(200);
    void *c = my_malloc(300);
    
    my_free(a);
    get_blocks_metadata();
    my_free(c);
    get_blocks_metadata();
    my_free(b);
    get_blocks_metadata();

    return 0;
}
