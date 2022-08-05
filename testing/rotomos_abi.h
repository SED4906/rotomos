#pragma once
#include <stddef.h>
extern size_t write(int fd, char* str, size_t len);
extern size_t read(int fd, char* str, size_t len);
extern void exit();
extern int open(char* path, char mode);
extern void close(int fd);
extern size_t mmap(size_t vaddr);
extern void munmap(size_t vaddr);
extern void exec(char* path);