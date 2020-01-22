
#include "xoracle.h"

#ifndef ROTL
#define ROTL(x,n) x = (x << (n)) | (x >> 32 - (n))
#endif

#ifndef ROTR
#define ROTR(x,n) x = (x >> (n)) | (x << 32 - (n))
#endif

#ifndef SHR
#define SHR(x,n) (x >> (n))
#endif

/*
static unsigned int constants[4] = {
    0x5a827999,
    0x6ed9eba1,
    0x8f1bbcdc,
    0xca62c1d6
};
*/

static void print_bits(size_t size, void* ptr) {
    unsigned char* b = (unsigned char *) ptr;

    for (size_t i = size - 1; i < ULONG_MAX; --i) {
        for (size_t j = 7; j < ULONG_MAX; --j) {
            unsigned char byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }

    printf("\n");
}

void testsha1(void) {
    const char* message = "abc";

    size_t length = strlen(message);
    size_t msgbits = length * 8;

    size_t blocks_needed = (length / 64) + 1;
    printf("Blocks needed: %lu\n", blocks_needed);

    size_t buffer_size = (blocks_needed * 64);

    unsigned char* buffer = allocate_memory(buffer_size);
    memset(buffer, 0, buffer_size);
    memcpy(buffer, message, length);

    unsigned char* m = (unsigned char *) message;
    unsigned char* p = buffer;

    while (*p++ = *m++)
        ;
    
    size_t padding_bits_needed = 448 - (length * 8) - 1;
    printf("Padding bits needed: %lu\n", padding_bits_needed);

    *p++ |= 1;

    size_t bytes_to_skip = (padding_bits_needed - 7) / 8;
    printf("Padding bytes to skip: %lu\n", bytes_to_skip);

    while (bytes_to_skip--) {
        *p++ &= 0;
    }

    *p |= msgbits;

    printf("%lx\n", (size_t) buffer);
    print_bits(buffer_size, buffer);
}
