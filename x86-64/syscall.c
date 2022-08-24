#include <x86-64/cpu.h>
#include <x86-64/mm.h>
#include <x86-64/fs.h>
#include <kernel/libc.h>
#include <x86-64/syscall.h>
#include <x86-64/task.h>
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
file_handle* open_handler(char* path) {
    file_handle* ret=0;
    if((ret = open_fifo(path))) return ret;
    if((ret = open_tar(path))) return ret;
    return 0;
}
void close_handler(file_handle* fd) {
    if(fd->fstype==0) close_fifo(fd);
    else if(fd->fstype==1) close_tar(fd);
}
size_t mmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == ((hhdm>>39)&0x1FF)) return 0;
    if(((vaddr>>39)&0x1FF) == 511) return 0;
    size_t page=alloc_page();
    if(!page) return 0;
    return map_page(get_pmap(),vaddr&~0xFFF,page,7);
}
void munmap_handler(size_t vaddr) {
    if(((vaddr>>39)&0x1FF) == ((hhdm>>39)&0x1FF)) return;
    if(((vaddr>>39)&0x1FF) == 511) return;
    size_t page=unmap_page(get_pmap(),vaddr&~0xFFF);
    if(page) dealloc_page(page&~0xFFF);
}
int exec_handler(char* path) {
    size_t size=0;
    size_t prev=0;
    file_handle* handle = open_handler(path);
    if(!handle) return 0;
    int i=0;
    do {
    prev=size;
    mmap_handler(((size_t)8<<39)+(4096*(size_t)i));
    } while((size += read_handler(handle,(char*)((size_t)8<<39)+(4096*((size_t)i++)),4096))!=prev);
    int ret=bin_exec(((size_t)8<<39),size);
    for(;i>=0;i--) munmap_handler(((size_t)8<<39)+(4096*((size_t)i-1)));
    close_handler(handle);
    return ret;
}

void init_syscall() {
    set_idt_entry(48,(void*)isr_write,0x8E);
    set_idt_entry(49,(void*)isr_read,0x8E);
    set_idt_entry(50,(void*)isr_exit,0x8E);
    set_idt_entry(51,(void*)isr_open,0x8E);
    set_idt_entry(52,(void*)isr_close,0x8E);
    set_idt_entry(53,(void*)isr_mmap,0x8E);
    set_idt_entry(54,(void*)isr_munmap,0x8E);
    set_idt_entry(55,(void*)isr_exec,0x8E);
}