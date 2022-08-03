
#include <kernel/core.h>
#include <kernel/exec.h>
#include <kernel/libc.h>
#include <stddef.h>

void flatbinary_exec(size_t file, size_t len) {
    uint64_t pmap = new_pmap();
    uint64_t stack = (uint64_t)kmalloc(1024);
    uint64_t hhdm=get_hhdm();
    for(size_t i=0;i<=(len>>12);i++) {
        uint64_t page=alloc_page();
        if(!page) hang_forever();
        if(i<(len>>12)) memcpy((void*)(page+hhdm),(void*)(file+i*4096),4096);
        else memcpy((void*)(page+hhdm),(void*)(file+i*4096),len&0xFFF);
        map_page(pmap, 0x800000+i*4096,page+i*4096,7);
    }
    *(uint64_t*)(stack+1016) = 0x800000;
    add_task(stack+1016,pmap);
}