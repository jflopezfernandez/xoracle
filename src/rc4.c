
#include "xoracle.h"

#ifndef RC4_DROP_N
#define RC4_DROP_N (3072)
#endif /* RC4_DROP_N */

static void swap_byte(unsigned char* a, unsigned char* b) {
    unsigned char temp = *a;
    *a = *b;
    *b = temp;
}

/** This function initializes the key-scheduling algorithm
 *  by scrambling the state vector in the key struct.
 */
void prepare_key(unsigned char* key_data_ptr, int key_data_len, rc4_key* key) {
    /* Initialize to the first element of the state vector */
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

/** This is the RC4 encryption cipher, with the added 
 *  modification of discarding the initial N bytes of the
 *  generated keystream (with the default setting being
 *  3072 discarded bytes).
 * 
 *  @todo Incorporate MAC
 *  @todo Incorporate hashed initialization vector (IV)
 * 
 *  @see http://web.archive.org/web/20080404222417/http://cypherpunks.venona.com/date/1994/09/msg00304.html
 *  @see http://users.zetnet.co.uk/hopwood/crypto/scan/cs.html#RC4-drop
 */
void rc4(unsigned char* buffer_ptr, int buffer_len, rc4_key* key) {
    unsigned char x = key->x;
    unsigned char y = key->y;

    unsigned char* state = &key->state[0];

    /** This counter keeps track of the number of bytes that
     *  have been discarded from the keystream so far. The
     *  minimum number must be 256, but the recommended
     *  amount is 3,072, which is the default.
     */
    size_t dropped_bytes = 0;

    for (int i = 0; i < buffer_len; ++i) {
        x = (x + 1) % 256;
        y = (state[x] + y) % 256;
        swap_byte(&state[x], &state[y]);

        if (dropped_bytes < RC4_DROP_N) {
            /* Increment the drop count */
            ++dropped_bytes;

            /** Decrement the loop counter to make sure we
             *  complete enough iterations to encrypt the
             *  entire input buffer.
             */
            --i;

            /** Move on to the next iteration without
             *  modifying the output buffer.
             */
            continue;
        }

        /* xor the keystream with the plaintext */
        unsigned char xor_index = (state[x] + state[y]) % 256;
        buffer_ptr[i] ^= state[xor_index];
    }

    key->x = x;
    key->y = y;
}
