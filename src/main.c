
#include "xoracle.h"

/** This character array is used as a lookup-table by the
 *  base64encode function.
 */
static char b64table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

unsigned char* base64encode(unsigned char* input, size_t length, int wrap) {
    size_t mod = length % 3;
    size_t i = 0;

    unsigned char* output = allocate_memory(((length / 3) + (mod ? 1 : 0)) * 4 + 1);
    unsigned char* p = output;

    while (i < (length - mod)) {
        /** Main loop body
         *
         *  This is where the main processing is done to
         *  convert three bytes of input into four bytes of
         *  output. 
         */
        *p++ = b64table[input[i++] >> 2];
        *p++ = b64table[((input[i - 1] << 4) | (input[i] >> 4)) & 0x3F];
        *p++ = b64table[((input[i] << 2) | (input[i + 1] >> 6)) & 0x3F];
        *p++ = b64table[input[i + 1] & 0x3F];

        /* Increment the loop counter */
        i += 2;

        /* Handle wrapping if the user requested it */
        if (wrap && !(i % 57)) {
            *p++ = '\n';
        }
    }

    /** Since every three bytes of input equal four bytes of
     *  input, inputs whose length is not a multiple of
     *  three require special handling to properly encode.
     *  This branch has special handling for whether the
     *  needed padding is one or two bytes long.
     */
    if (mod) {
        *p++ = b64table[input[i++] >> 2];
        *p++ = b64table[((input[i-1] << 4) | (input[i] >> 4)) & 0x3F];

        if (mod == 1) {
            *p++ = '=';
            *p++ = '=';
        } else {
            *p++ = b64table[(input[i] << 2) & 0x3F];
            *p++ = '=';
        }
    }

    /** If the user requested wrapping, ensure the string
     *  contains a final newline before the null terminator.
     */
    if (wrap) {
        *p++ = '\n';
    }

    /* Null-terminate the encoded output string */
    *p = '\0';

    return output;
}

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "No input(s).\n");
        return EXIT_FAILURE;
    }

    while (*++argv) {
        size_t length = strlen(*argv);
        unsigned char* encoded = base64encode((unsigned char *) *argv, length, 0);
        printf("%s: %s\n", *argv, encoded);
        free(encoded);
    }

    return EXIT_SUCCESS;
}
