#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
int strlen(const char* str);
int printf(char const* format, ...);

#ifdef __cplusplus
}
#endif