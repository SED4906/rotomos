#include <kernel/libc.h>
#include <kernel/core.h>
#include <limine.h>
#include <stddef.h>
typedef void* freelist;
freelist* free_pages;
uint64_t hhdm;

size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = (freelist*)*(freelist*)((uint64_t)free_pages + hhdm);
    return ret;
}

void dealloc_page(size_t page) {
    freelist* entry = (freelist*)page;
    *(freelist*)((uint64_t)entry + hhdm) = free_pages;
    free_pages = entry;
}

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0, .response = 0
};

struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0, .response = 0
};

typedef struct heap {
    int64_t bytes;
    struct heap* next;
} heap;

heap* kernel_heap;

void init_mm() {
    if(!memmap_request.response) hang_forever();
    if(!hhdm_request.response) hang_forever();
    hhdm = hhdm_request.response->offset;

    for(size_t e=0;e<memmap_request.response->entry_count;e++) {
        struct limine_memmap_entry* en=memmap_request.response->entries[e];
        printf("%X: %x->%x %X\n", e, en->base, en->base + en->length, en->type);
        if(en->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t o=0;o<en->length;o+=4096) dealloc_page(en->base + o);
    }

    kernel_heap = (heap*)((size_t)4 << 39);
    uint64_t current_pmap = get_pmap();
    for(int i=0;i<128;i++) {
        uint64_t page;
        if(!(page = alloc_page())) hang_forever();
        map_page(current_pmap, (size_t)kernel_heap+i*4096, page, 7);
    }

    kernel_heap->bytes = 128*4096 - sizeof(heap);
    kernel_heap->next = 0;
}

size_t map_page_step(size_t pmap, size_t entry) {
    if(((size_t*)((uint64_t)pmap + hhdm))[entry] & 1) return ((size_t*)((uint64_t)pmap + hhdm))[entry] &~ 0xFFF;
    size_t ret = alloc_page();
    memset((void*)(ret+hhdm),0,4096);
    ((size_t*)((uint64_t)pmap + hhdm))[entry] = ret | 7;
    return ret;
}

size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    pml3 = map_page_step(pmap, pml4_entry);
    pml2 = map_page_step(pml3, pml3_entry);
    pml1 = map_page_step(pml2, pml2_entry);
    ((size_t*)((uint64_t)pml1 + hhdm))[pml1_entry] = paddr | flags;
    return vaddr;
}

void unmap_page(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(((size_t*)((uint64_t)pmap + hhdm))[pml4_entry] & 1) pml3 = map_page_step(pmap, pml4_entry); else return;
    if(((size_t*)((uint64_t)pml3 + hhdm))[pml3_entry] & 1) pml2 = map_page_step(pml3, pml3_entry); else return;
    if(((size_t*)((uint64_t)pml2 + hhdm))[pml2_entry] & 1) pml1 = map_page_step(pml2, pml2_entry); else return;
    ((size_t*)((uint64_t)pml1 + hhdm))[pml1_entry] &=~ 1;
    tlb_invalidate(vaddr);
}

void* kmalloc(size_t bytes) {
    for(heap* heap_entry = kernel_heap; heap_entry; heap_entry = heap_entry->next) {
        if(heap_entry->bytes >= (int64_t)bytes) {
            if(heap_entry->bytes - bytes > sizeof(heap)) {
                size_t remaining = heap_entry->bytes - bytes - sizeof(heap);
                heap* next_next = heap_entry->next;
                heap_entry->bytes = -bytes;
                heap_entry->next = (heap*)((size_t)heap_entry + bytes + sizeof(heap));
                heap_entry->next->bytes = remaining;
                heap_entry->next->next = next_next;
                return heap_entry + 1;
            } else {
                heap_entry->bytes = -heap_entry->bytes;
                return heap_entry + 1;
            }
        }
    }
    return 0;
}

void kdemalloc(void* data) {
    heap* heap_entry = (heap*)((size_t)data - sizeof(heap));
    heap_entry->bytes = -heap_entry->bytes;
}

size_t new_pmap() {
    uint64_t ret=0;
    if(!(ret=alloc_page())) return 0;
    memset((void*)(ret+hhdm),0,4096);
    uint64_t* retp = (uint64_t*)ret;
    uint64_t* pmap = (uint64_t*)get_pmap();
    //retp[0] = pmap[0];
    retp[4] = pmap[4];
    retp[(hhdm>>39)&0x1FF] = pmap[(hhdm>>39)&0x1FF];
    retp[511] = pmap[511];
    return ret;
}

size_t get_hhdm() {
    return hhdm;
}