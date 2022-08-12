#pragma once
#include <stddef.h>
extern size_t write(void* fd, char* str, size_t len);
extern size_t read(void* fd, char* str, size_t len);
extern void exit();
extern void* open(char* path, char mode);
extern void close(void* fd);
extern size_t mmap(size_t vaddr);
extern void munmap(size_t vaddr);
extern void exec(char* path);