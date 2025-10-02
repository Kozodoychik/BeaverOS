#include <stdint.h>
#include <stdbool.h>


typedef struct allocated_memory_header allocated_memory_header;

struct allocated_memory_header {
    uint32_t size;
    bool is_free;

    allocated_memory_header* next;
    allocated_memory_header* prev;
};

void init_heap(uint32_t base, uint32_t size);

void* malloc(uint32_t size);
void free(void* memory);