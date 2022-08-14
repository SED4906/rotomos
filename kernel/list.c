#include <kernel/list.h>
#define OFFSET_VOIDPTR(x,o) (void*)*(size_t*)((char*)(x) + o)

void* list_last(void* list, size_t offset_next) {
    if(!list) return 0;
    void* current = list;
    void* next = OFFSET_VOIDPTR(current, offset_next);
    while (next) {
        current = next;
        next = OFFSET_VOIDPTR(current, offset_next);
    }
    return current;
}

void* list_last_callback(void* list, size_t offset_next, void (*callback)(void*)) {
    if(!list) return 0;
    void* current = list;
    void* next = OFFSET_VOIDPTR(current, offset_next);
    while (next) {
        callback(current);
        current = next;
        next = OFFSET_VOIDPTR(current, offset_next);
    }
    return current;
}