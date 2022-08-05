#include <kernel/core.h>
#include <kernel/exec.h>
#include <kernel/fs.h>
#include <kernel/keyb.h>
#include <kernel/libc.h>
#include <stddef.h>

size_t write_handler(int fd, char* str, size_t len) {
    if(fd == 1) {
        return fb_print_string(str, len);
    } else {
        return write_tar(fd, str, len);
    }
}
size_t read_handler(int fd, char* str, size_t len) {
    if(fd == 0) {
        size_t i;
        for(i=0;i<len;i++) {
            if(!(str[i] = keyb_readnext())) return i;
        }
        return i;
    } else {
        return read_tar(fd, str, len);
    }
}
void exit_handler() {
    exit_task();
}
int open_handler(char* path, char mode) {
    return open_tar(path, mode);
}
void close_handler(int fd) {
    close_tar(fd);
}
size_t mmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == 4) return 0;
    if(((vaddr>>39)&0x1FF) == ((get_hhdm()>>39)&0x1FF)) return 0;
    if(((vaddr>>39)&0x1FF) == 511) return 0;
    size_t page=alloc_page();
    if(!page) return 0;
    memset((void*)(page+get_hhdm()),0,4096);
    return map_page(get_pmap(),vaddr&~0xFFF,page,7);
}
void munmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == 4) return;
    if(((vaddr>>39)&0x1FF) == ((get_hhdm()>>39)&0x1FF)) return;
    if(((vaddr>>39)&0x1FF) == 511) return;
    size_t page=unmap_page(get_pmap(),vaddr&~0xFFF);
    if(page) dealloc_page(page&~0xFFF);
}
void exec_handler(char* path) {
    size_t size=0;
    size_t prev=0;
    int fd = open_handler(path,'r');
    if(fd == -1) return;
    int i=0;
    do {
    prev=size;
    mmap_handler(((size_t)8<<39)+(4096*(size_t)i++));
    } while((size += read_handler(fd,(char*)((size_t)8<<39)+(4096*((size_t)i-1)),4096))!=prev);
    flatbinary_exec(((size_t)8<<39),size);
    for(;i>=0;i--) munmap_handler(((size_t)8<<39)+(4096*((size_t)i-1)));
    close_handler(fd);
}