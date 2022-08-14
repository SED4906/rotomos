#include <kernel/mm.h>
#include <kernel/exec.h>
#include <kernel/fs.h>
#include <kernel/keyb.h>
#include <kernel/libc.h>
#include <kernel/task.h>
#include <stddef.h>

size_t write_handler(file_handle* fd, char* str, size_t len) {
    if(fd->fstype == 0) {
        return write_fifo(fd, str, len);
    } else if(fd->fstype == 1) {
        return write_tar(fd, str, len);
    }
    return 0;
}
size_t read_handler(file_handle* fd, char* str, size_t len) {
    if(fd->fstype == 0) {
        return read_fifo(fd, str, len);
    } else if(fd->fstype == 1) {
        return read_tar(fd, str, len);
    }
    return 0;
}
void exit_handler() {
    exit_task();
}
file_handle* open_handler(char* path, char mode) {
    file_handle* ret=0;
    if((ret = open_fifo(path, mode))) return ret;
    if((ret = open_tar(path, mode))) return ret;
    return 0;
}
void close_handler(file_handle* fd) {
    if(fd->fstype==0) close_fifo(fd);
    else if(fd->fstype==1) close_tar(fd);
}
size_t mmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == ((get_hhdm()>>39)&0x1FF)) return 0;
    if(((vaddr>>39)&0x1FF) == 511) return 0;
    size_t page=alloc_page();
    if(!page) return 0;
    return map_page(get_pmap(),vaddr&~0xFFF,page,7);
}
void munmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == ((get_hhdm()>>39)&0x1FF)) return;
    if(((vaddr>>39)&0x1FF) == 511) return;
    size_t page=unmap_page(get_pmap(),vaddr&~0xFFF);
    if(page) dealloc_page(page&~0xFFF);
}
void exec_handler(char* path) {

    size_t size=0;
    size_t prev=0;
    file_handle* handle = open_handler(path,'r');
    if(!handle) return;
    int i=0;
    do {
    prev=size;
    mmap_handler(((size_t)8<<39)+(4096*(size_t)i));
    } while((size += read_handler(handle,(char*)((size_t)8<<39)+(4096*((size_t)i++)),4096))!=prev);
    flatbinary_exec(((size_t)8<<39),size);
    for(;i>=0;i--) munmap_handler(((size_t)8<<39)+(4096*((size_t)i-1)));
    close_handler(handle);
}