#pragma once
#include <stddef.h>
//// core/libc.c
void* memcpy(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
int strlen(const char* str);
//// core/printf.c
int printf(char const* format, ...);