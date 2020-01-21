
#include "xoracle.h"

/** This character array is used as a lookup-table by the
 *  base64encode function.
 */
static char b64table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "abcdefghijklmnopqrstuvwxyz"
                           "0123456789+/";

/** This function converts the input buffer into base64
 *  encoding. It does not modify the original buffer, but
 *  rather returns a new buffer, which the caller must then
 *  free.
 */
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

/** This is the lookup table used by the raw_base64_decode
 *  function, which performs the actual base64 decoding of
 *  the input string.
 */
static char b64revtb[256] = {
    -3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*   0-15 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /*  16-31 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, /*  32-47 */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /*  48-63 */
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /*  64-79 */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /*  80-95 */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /*  96-111 */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* 112-127 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 128-143 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 144-159 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 160-175 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 176-191 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 192-207 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 208-223 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 224-239 */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 240-255 */
};

static size_t raw_base64_decode(unsigned char* input, unsigned char* output, int strict, int* error) {
    /** Reset the error value in case it contains an error
     *  from a previous function call.
     */
    *error = 0;

    /** Variable to indicate to the caller the length of the
     *  decoded string. The original (pre-base64 encoded)
     *  string could have been binary data, containing zero-
     *  valued bytes (\x00), which would be interpreted by
     *  library functions as null-terminators. To prevent
     *  this, we explicitly return the size of the decoded
     *  buffer.
     */
    size_t output_length = 0;

    /** Pointer variable for iterating over the input buffer
     *  character by character.
     */
    unsigned char* p = input;

    unsigned char buffer[3] = { 0 };

    unsigned char pad = 0;

    int x = 0;

    while (!pad) {
        switch ((x = b64revtb[*p++])) {
            /* Null-terminator */
            case -3: {
                if (((p - 1) - input) % 4) {
                    *error = 1;
                }

                /** Check that a pad is actually required,
                 *  otherwise strings of a length that
                 *  is a multiple of three would trigger the
                 *  for loop 3-pad times (which with a pad
                 *  of zero would equal 3). This would then
                 *  cause the buffer containing the last
                 *  three characters of the output string to
                 *  get printed twice, i.e... hello!lo!
                 * 
                 *  @bug Strings of a length other than
                 *  three are now again missing the last bit
                 *  of the buffer.
                 */
                if (pad) {
                    for (x = 0; x < 3 - pad; x++) {
                        *output++ = buffer[x];
                    }
                }

                return output_length;
            } break;

            /* Padding character. Invalid here */
            case -2: {
                if (((p - 1) - input) % 4 < 2) {
                    *error = 1;
                    return output_length;
                } else if (((p - 1) - input) % 4 == 2) {
                    /** Make sure the input string is
                     *  appropriately padded.
                     */
                    if (*p != '=') {
                        *error = 1;
                        return output_length;
                    }

                    buffer[2] = 0;
                    pad = 2;
                    ++output_length;
                    break;
                } else {
                    pad = 1;
                    output_length += 2;
                    break;
                }

                fprintf(stderr, "Exiting via call to case -2\n");

                return output_length;
            } break;

            /* Invalid character in the input */
            case -1: {
                /* TODO: Make strict behavior the default */
                if (strict) {
                    *error = 2;
                    return output_length;
                }
            } break;

            default: {
                switch (((p - 1) - input) % 4) {
                    case 0: {
                        buffer[0] = x << 2;
                    } break;

                    case 1: {
                        buffer[0] |= (x >> 4);
                        buffer[1]  = (x << 4);
                    } break;

                    case 2: {
                        buffer[1] |= (x >> 2);
                        buffer[2]  = (x << 6);
                    } break;

                    case 3: {
                        buffer[2] |= x;
                        output_length += 3;

                        for (x = 0; x < 3 - pad; x++) {
                            *output++ = buffer[x];
                        }
                    } break;
                }
            } break;
        }
    }

    for (x = 0; x < 3 - pad; x++) {
        *output++ = buffer[x];
    }

    return output_length;
}

/** base64decode
 * 
 *  [Description]
 * 
 *  Error Values:
 * 
 *      -1  Padding error
 *      -2  Strict checking requested (and failed)
 *      -3  malloc() failed
 * 
 *  Notes:
 * 
 *      In the current implementation of allocate_memory,
 *      any failure to allocate heap memory is handled by
 *      immediately exiting the program. There is therefore
 *      no way for base64decode to return a -3 error code,
 *      as program execution will not continue far enough to
 *      do so in that scenario.
 */
unsigned char* base64decode(unsigned char* input, size_t* length, int strict, int* error) {
    /* Allocate memory block for the decoded string */
    unsigned char* output = allocate_memory(3 * (strlen((const char*) input) / 4 + 1));

    /* Call the decoding function */
    *length = raw_base64_decode(input, output, strict, error);

    /** Should raw_base64_decode fail for any reason, it
     *  will modify the error variable with the appropriate
     *  code. It is therefore important to check this after
     *  each call to raw_base64_decode.
     */
    if (*error) {
        /* Free the heap memory */
        free(output);

        /** Make sure the output variable no longer points
         *  to the address that held the now-invalidated
         *  data buffer.
         */
        output = NULL;

        /** Set the length parameter to zero, as the reply
         *  buffer now contains nothing.
         */
        *length = 0;
    }

    return output;
}
