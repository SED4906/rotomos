#pragma once
#include <stddef.h>
extern size_t write(int fd, char* str, size_t len);
extern size_t read(int fd, char* str, size_t len);
extern void exit();
extern size_t mmap(size_t vaddr);
extern void munmap(size_t vaddr);