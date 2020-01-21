
#ifndef PROJECT_INCLUDES_BASE64_H
#define PROJECT_INCLUDES_BASE64_H

unsigned char* base64encode(unsigned char* input, size_t length, int wrap);

unsigned char* base64decode(unsigned char* input, size_t* length, int strict, int* error);

#endif /* PROJECT_INCLUDES_BASE64_H */
