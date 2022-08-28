#pragma once
#include <x86-64/fs.h>
size_t read_handler(file_handle* handle, char* buffer, size_t length);
file_handle* open_handler(char* path);
int exec_handler(char* path, char* cmdline);
void init_syscall();
extern size_t isr_write(void* handle, void* buffer, size_t length);
extern size_t isr_read(void* handle, void* buffer, size_t length);
extern void isr_exit();
extern size_t isr_open(char* path);
extern size_t isr_close(void* handle);
extern size_t isr_mmap(void* address);
extern size_t isr_munmap(void* address);
extern int isr_exec(char* path, char* cmdline);

void init_keyb();
extern void keyb_interrupt();