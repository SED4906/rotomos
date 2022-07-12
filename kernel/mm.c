#include <kernel/libc.h>
#include <kernel/mm.h>
#include <limine.h>
#include <stddef.h>
typedef void* freelist;
freelist* free_pages;
/// Allocate Page
size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = (freelist*)*free_pages;
    return ret;
}
/// Deallocate Page
void dealloc_page(size_t page) {
    freelist* entry = (freelist*)page;
    *entry = free_pages;
    free_pages = entry;
}
struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0, .response = 0
};
/// Initialize Page Freelist
void init_page() {
    if(!memmap_request.response) for(;;) __asm__ volatile("cli;hlt");

    for(size_t e=0;e<memmap_request.response->entry_count;e++) {
        struct limine_memmap_entry* en=memmap_request.response->entries[e];
        printf("%X: %x->%x %X\n", e, en->base, en->base + en->length, en->type);
        if(en->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t o=0;o<en->length;o+=4096) dealloc_page(en->base + o);
    }
}

size_t map_page_step(size_t pmap, size_t entry) {
    if(((size_t*)pmap)[entry] & 1) return ((size_t*)pmap)[entry] &~ 0xFFF;
    size_t ret = alloc_page();
    ((size_t*)pmap)[entry] = ret | 7;
    return ret;
}
/// Map Page
size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    pml3 = map_page_step(pmap, pml4_entry);
    pml2 = map_page_step(pml3, pml3_entry);
    pml1 = map_page_step(pml2, pml2_entry);
    ((size_t*)pml1)[pml1_entry] = paddr | flags;
    return vaddr;
}
/// Unmap Page
void unmap_page(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(((size_t*)pmap)[pml4_entry] & 1) pml3 = map_page_step(pmap, pml4_entry); else return;
    if(((size_t*)pmap)[pml3_entry] & 1) pml2 = map_page_step(pml3, pml3_entry); else return;
    if(((size_t*)pmap)[pml2_entry] & 1) pml1 = map_page_step(pml2, pml2_entry); else return;
    ((size_t*)pml1)[pml1_entry] &=~ 1;
}

/// Kernel Memory Allocate
// TODO - Implement a proper heap
void* kmalloc(size_t bytes) {
    if(bytes > 4096) return 0;
    return (void*)alloc_page();
}
/// Kernel Memory Deallocate
void kdemalloc(void* data) {
    dealloc_page((size_t)data);
}