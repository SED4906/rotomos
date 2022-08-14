#pragma once
#include <stddef.h>

//! @brief Keep traversing a linked list until the end.
//! @param list A linked list.
//! @param offset_next The offset, in bytes, of list->next in the structure.
//! @return The last entry found.
void* list_last(void* list, size_t offset_next);

//! @brief Keep traversing a linked list until the end, calling a function on each entry except for the last.
//! @param list A linked list.
//! @param offset_next The offset, in bytes, of list->next in the structure.
//! @param callback A function to call, passing each found entry to it.
//! @return The last entry found.
void* list_last_callback(void* list, size_t offset_next, void (*callback)(void*));