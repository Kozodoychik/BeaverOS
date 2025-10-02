#include <stdint.h>
#include <string.h>

uint8_t* memcpy(uint8_t* dst, uint8_t* src, int size) {
    for (int i=0;i<size;i++) {
        dst[i] = src[i];
    }
    return dst;
}

uint8_t* memset(uint8_t* ptr, uint8_t value, int size) {
    for (int i=0;i<size;i++) {
        ptr[i] = value;
    }
    return ptr;
}