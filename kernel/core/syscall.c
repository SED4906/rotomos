#include <kernel/core.h>
#include <kernel/libc.h>
#include <stddef.h>

size_t write_handler(int fd, char* str, size_t len) {
    if(fd == 1) {
        return fb_print_string(str, len);
    } else {
        return 0;
    }
}
size_t read_handler(int fd, char* str, size_t len) {
    if(fd == 0) {
        for(size_t i=0;i<len;i++) {
            if(!(str[i] = keyb_readnext())) return i;
        }
        return len;
    } else {
        return 0;
    }
}
void exit_handler() {
    exit_task();
}
int open_handler(char* path, char mode) {
    return -1;
}
void close_handler(int fd) {
}
size_t mmap_handler(size_t vaddr) {
    if(vaddr>>39 == 4) return 0;
    if(vaddr>>39 == ((get_hhdm()>>39)&0x1FF)) return 0;
    if(vaddr>>39 == 511) return 0;
    size_t page=alloc_page();
    if(!page) return 0;
    map_page(get_pmap(),vaddr&~0xFFF,page,7);
    return 0;
}
void munmap_handler(size_t vaddr) {
    if(vaddr>>39 == 4) return;
    if(vaddr>>39 == ((get_hhdm()>>39)&0x1FF)) return;
    if(vaddr>>39 == 511) return;
    size_t page=unmap_page(get_pmap(),vaddr&~0xFFF);
    if(page) dealloc_page(page&~0xFFF);
}
void exec_handler(char* path) {
}