#include <kernel/core.h>
#include <kernel/libc.h>
#include <stddef.h>

size_t create_pagemap() {
    size_t* pagemap = (size_t*)alloc_page();
    if(!pagemap) return 0;
    memset(pagemap, 0, 4096);
    size_t* current;
    __asm__ volatile("movq %%cr3, %0"::"=r"(current));
    pagemap[0] = current[0];
    pagemap[511] = current[511];
    return (size_t)pagemap;
}

#define PROCESS_LOAD_START ((size_t)1 << 39)
#define STACK_START ((size_t)2 << 39)

void aout_exec(void* data, size_t len) {
    size_t a_text = ((size_t*)data)[1];
    size_t a_data = ((size_t*)data)[2];
    size_t a_bss = ((size_t*)data)[3];
    size_t pagemap = create_pagemap();
    if(!pagemap) for(;;) __asm__ volatile("hlt");
    size_t pages = (a_bss+len)>>12;
    for(size_t ent = 0; ent<pages; ent++) {
        size_t page = alloc_page();
        if(!page) for(;;) __asm__ volatile("hlt");
        map_page(pagemap, PROCESS_LOAD_START + (ent<<12), page, 7);
    }
    memcpy(PROCESS_LOAD_START, data, len);
    memset((void*)((size_t)PROCESS_LOAD_START+len), 0, a_bss);

    size_t stackpages = 16;
    for(size_t ent = 0; ent<stackpages; ent++) {
        size_t page = alloc_page();
        if(!page) for(;;) __asm__ volatile("hlt");
        map_page(pagemap, STACK_START + (ent<<12), page, 7);
    }

    size_t* retvec = (size_t*)(STACK_START+(stackpages*4096)-8);
    *retvec = PROCESS_LOAD_START+64;
    add_task(STACK_START+(stackpages*4096)-8,pagemap);
}