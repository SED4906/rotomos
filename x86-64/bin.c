#include <kernel/libc.h>
#include <x86-64/cpu.h>
#include <x86-64/mm.h>
#include <x86-64/task.h>

int bin_exec(size_t file, size_t len, char* cmdline) {
    uint64_t pmap = new_pmap();
    uint64_t stack = (uint64_t)page_allocate(0);
    for(size_t i=0;i<=(len>>12);i++) {
        uint64_t page=alloc_page();
        if(!page) die();
        if(i<(len>>12)) memcpy((void*)(page+hhdm),(void*)(file+i*4096),4096);
        else memcpy((void*)(page+hhdm),(void*)(file+i*4096),len&0xFFF);
        map_page(pmap, 0x800000+i*4096,page+i*4096,7);
    }
    *(uint64_t*)(stack+4088) = 0x800000;
    size_t* r = (size_t*)heap_allocate(sizeof(size_t)*16);
    r[7] = (size_t)cmdline;
    int ret = add_task(stack+4088,pmap,r);
    heap_deallocate(r);
    return ret;
}