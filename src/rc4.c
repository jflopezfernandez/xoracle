
#include "xoracle.h"

static void swap_byte(unsigned char* a, unsigned char* b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

void prepare_key(unsigned char* key_data_ptr, int key_data_len, rc4_key* key) {
    unsigned char* state = &key->state[0];

    /* Identity permutation */
    for (int i = 0; i < 256; ++i) {
        state[i] = i;
    }

    key->x = 0;
    key->y = 0;

    unsigned char index_1 = 0;
    unsigned char index_2 = 0;

    for (int i = 0; i < 256; ++i) {
        index_2 = (key_data_ptr[index_1] + state[i] + index_2) % 256;
        swap_byte(&state[i], &state[index_2]);
        index_1 = (index_1 + 1) % key_data_len;
    }
}

void rc4(unsigned char* buffer_ptr, int buffer_len, rc4_key* key) {
    unsigned char x = key->x;
    unsigned char y = key->y;

    unsigned char* state = &key->state[0];

    for (int i = 0; i < buffer_len; ++i) {
        x = (x + 1) % 256;
        y = (state[x] + y) % 256;
        swap_byte(&state[x], &state[y]);
        unsigned char xor_index = (state[x] + state[y]) % 256;
        buffer_ptr[i] ^= state[xor_index];
    }

    key->x = x;
    key->y = y;
}
