
#include "xoracle.h"

/** Prevent the creation of a core file should the program
 *  encounter any critical errors during execution. Core
 *  files could potentially contain sensitive information
 *  that could be recovered by a hostile actor.
 */
static void disable_memory_dumping(void) {
    struct rlimit resource_limit;
    resource_limit.rlim_cur = 0;
    resource_limit.rlim_max = 0;

    /* Clear the error flag before calling setrlimit */
    errno = 0;

    if (setrlimit(RLIMIT_CORE, &resource_limit)) {
        fprintf(stderr, "[Fatal Error] %s: %s\n", "Could not disable memory dumps", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    disable_memory_dumping();

    if (argc == 1) {
        fprintf(stderr, "No input(s).\n");
        return EXIT_FAILURE;
    } else if (strcmp(*++argv, "encode") == 0) {
        while (*++argv) {
            size_t length = strlen(*argv);
            unsigned char* encoded = base64encode((unsigned char *) *argv, length, 0);
            printf("%s\n", (char *) encoded);
            free(encoded);
        }
    } else if (strcmp(*argv, "decode") == 0) {
        while (*++argv) {
            size_t length = 0;
            int error = 0;
            unsigned char* decoded = base64decode((unsigned char *) *argv, &length, 0, &error);
            printf("%s\n", (char *) decoded);
            free(decoded);
        }
    } else if (strcmp(*argv, "random") == 0) {
        size_t length = 32;
        unsigned char* buffer = allocate_memory(length);
        /*ssize_t status = */getrandom(buffer, length, 0);
        unsigned char* encoded = base64encode(buffer, length, 0);
        printf("%s\n", (char *) buffer);
        printf("%s\n", (char *) encoded);
        free(buffer);
        free(encoded);
    } else if (strcmp(*argv, "encrypt") == 0) {
        while (*++argv) {
            unsigned char* key_data = (unsigned char *) "password";
            size_t key_data_length = strlen((char *) key_data);
            rc4_key key;
            prepare_key(key_data, key_data_length, &key);

            size_t length = strlen(*argv);
            unsigned char* buffer = allocate_memory(length + 1);
            rc4(buffer, length, &key);

            unsigned char* encoded = base64encode(buffer, length, 0);
            printf("%s\n", (char *) buffer);
            printf("%s\n", (char *) encoded);

            free(buffer);
            free(encoded);
        }
    } else if (strcmp(*argv, "sha1") == 0) {
        testsha1();
    } else {
        fprintf(stderr, "Unknown operation: %s\n", *argv);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
