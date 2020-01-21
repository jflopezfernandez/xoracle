
#include "xoracle.h"

void* allocate_memory(size_t size) {
    void* memory_block = malloc(size);

    if (memory_block == NULL) {
        fprintf(stderr, "[Error] Memory allocation failure\n");
        exit(EXIT_FAILURE);
    }

    return memory_block;
}
