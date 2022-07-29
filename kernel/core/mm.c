#include <kernel/libc.h>
#include <kernel/core.h>
#include <limine.h>
#include <stddef.h>
typedef void* freelist;
freelist* free_pages;
//! @brief Unlink page from freelist.
//! @return A page-aligned address in physical memory.
size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = (freelist*)*free_pages;
    return ret;
}

//! @brief Links page to freelist.
//! @param page A page-aligned address in physical memory.
void dealloc_page(size_t page) {
    freelist* entry = (freelist*)page;
    *entry = free_pages;
    free_pages = entry;
}

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0, .response = 0
};

//! @brief If a map of physical memory is unavailable, halts.
//! All usable memory should be added to the freelist.
void init_mm() {
    if(!memmap_request.response) hang_forever();

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

//! @brief Sets/updates a memory mapping, allocating memory if necessary.
//! @param pmap A page map.
//! @param vaddr A virtual address.
//! @param paddr A physical address.
//! @param flags Flags.
//! @return The same virtual address.
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

//! @brief Removes a memory mapping if it exists.
//! @param pmap A page map.
//! @param vaddr A virtual address.
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
    tlb_invalidate(vaddr);
}

//! @todo Implement a proper heap.
void* kmalloc(size_t bytes) {
    if(bytes > 4096) return 0;
    return (void*)alloc_page();
}
//! @todo Implement a proper heap.
void kdemalloc(void* data) {
    dealloc_page((size_t)data);
}