
#include "xoracle.h"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "No input(s).\n");
        return EXIT_FAILURE;
    }

    while (*++argv) {
        printf("%s\n", *argv);

        size_t length = strlen(*argv);
        unsigned char* encoded = base64encode((unsigned char *) *argv, length, 0);
        printf("%s\n", encoded);

        int error = 0;
        unsigned char* decoded = base64decode((unsigned char *) encoded, &length, 0, &error);
        printf("%s\n", (char *) decoded);

        free(encoded);
        free(decoded);
    }

    return EXIT_SUCCESS;
}
