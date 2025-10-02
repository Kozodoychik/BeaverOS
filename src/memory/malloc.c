#include <memory/malloc.h>


allocated_memory_header* heap_base;

void init_heap(uint32_t base, uint32_t size) {
    allocated_memory_header* h = (allocated_memory_header*)base;

    h->size = size;
    h->is_free = true;
    h->next = 0;
    h->prev = 0;

    heap_base = h;
}

void* malloc(uint32_t size) {
    allocated_memory_header* h = heap_base;

    while((h->size < size || !h->is_free) && h->next != 0) {

        if (h->is_free && h->next != 0 && h->next->is_free) {
            h->size += h->next->size + sizeof(allocated_memory_header);
            h->next = h->next->next;

            h = h->prev;
        }

        h = h->next;
    }

    if (h->next == 0 && (!h->is_free || h->size < size)) return 0;

    h->next = (allocated_memory_header*)((uint32_t)h + sizeof(allocated_memory_header) + size);
    h->is_free = false;

    allocated_memory_header* next = h->next;

    next->is_free = true;
    next->size = h->size - sizeof(allocated_memory_header) - size;
    next->prev = h;
    next->next = 0;

    return (void*)((uint32_t)h + sizeof(allocated_memory_header));
}

void free(void* ptr) {

}