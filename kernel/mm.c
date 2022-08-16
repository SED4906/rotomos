#include <kernel/cpu.h>
#include <kernel/libc.h>
#include <kernel/mm.h>
#include <limine.h>
#include <stddef.h>
typedef void* freelist;
freelist* free_pages;
uint64_t hhdm;

size_t alloc_page() {
    size_t ret = (size_t)free_pages;
    free_pages = (freelist*)*(freelist*)((uint64_t)free_pages + hhdm);
    memset((void*)(ret+hhdm),0,4096);
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
    char bitmap[448];
    struct heap* next;
    struct heap* prev;
} heap;

typedef struct page {
    void* address;
    int refs;
    struct page* next;
    struct page* prev;
} page;

heap* kernel_heap;
page* kernel_blk;

void* heap_allocate(size_t bytes) {
    if(bytes > 4096-514) return 0;
    size_t run=0;
    heap* check=kernel_heap;
    int i=0;
    while(check) {
        for(i=0;i<448*8;i++) {
            if(check->bitmap[i>>3] & (1<<(i&7))) run=0;
            else run++;
            if(run == bytes + 2) break;
        }
        if(run == bytes + 2) break;
        run=0;
        if(!check->next) {
            heap* ent=(heap*)alloc_page();
            if(!ent) return 0;
            ent = (heap*)((size_t)(ent)+hhdm);
            ent->prev=check;
            check->next=ent;
            ent->next=0;
        }
        check = check->next;
    }
    if(!check) return 0;
    for(int j=i-bytes;j<=i;j++) {
        check->bitmap[j>>3] |= 1<<(j&7);
    }
    return (char*)(check)+512+i-bytes;
}

void heap_deallocate(void* ptr) {
    heap* check=(heap*)((size_t)(ptr)&~0xFFF);
    if(check == ptr) return;
    size_t i=((size_t)(ptr)&0xFFF)-512;
    while(check->bitmap[i>>3] & (1<<(i&7))) {
        check->bitmap[i>>3] ^= 1<<(i&7);
        i++;
    }
}

void* page_allocate(void* address) {
    if(!address) {
        page* ent = heap_allocate(sizeof(page));
        if(!ent) return 0;
        ent->address = (void*)(alloc_page()+hhdm);
        ent->next=kernel_blk;
        if(kernel_blk) kernel_blk->prev=ent;
        kernel_blk=ent;
        ent->prev=0;
        ent->refs=1;
        return ent->address;
    }
    page* check = kernel_blk;
    while(check && check->address != address) check=check->next;
    if(!check) return 0;
    check->refs++;
    return check->address;
}

void page_deallocate(void* address) {
    if(!address) return;
    page* check = kernel_blk;
    while(check && check->address != address) check=check->next;
    if(!check) return;
    check->refs--;
    if(!check->refs) {
        dealloc_page((size_t)(check->address)-hhdm);
        if(check->prev) check->prev->next = check->next;
        if(check->next) check->next->prev = check->prev;
        heap_deallocate(check);
    }
}

void init_mm() {
    if(!memmap_request.response) hang_forever();
    if(!hhdm_request.response) hang_forever();
    hhdm = hhdm_request.response->offset;
    size_t found_kilobytes=0;

    for(size_t e=0;e<memmap_request.response->entry_count;e++) {
        struct limine_memmap_entry* en=memmap_request.response->entries[e];
        //printf("%X: %x->%x %X\n", e, en->base, en->base + en->length, en->type);
        if(en->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t o=0;o<en->length;o+=4096) dealloc_page(en->base + o);
        found_kilobytes += en->length >> 10;
    }
    printf("Usable memory: %dKB\n", found_kilobytes);
    kernel_heap = (heap*)(alloc_page()+hhdm);
    kernel_heap->next=0;kernel_heap->prev=0;
    kernel_blk = 0;
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

size_t unmap_page(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(((size_t*)((uint64_t)pmap + hhdm))[pml4_entry] & 1) pml3 = map_page_step(pmap, pml4_entry); else return 0;
    if(((size_t*)((uint64_t)pml3 + hhdm))[pml3_entry] & 1) pml2 = map_page_step(pml3, pml3_entry); else return 0;
    if(((size_t*)((uint64_t)pml2 + hhdm))[pml2_entry] & 1) pml1 = map_page_step(pml2, pml2_entry); else return 0;
    ((size_t*)((uint64_t)pml1 + hhdm))[pml1_entry] &=~ 1;
    tlb_invalidate(vaddr);
    return ((size_t*)((uint64_t)pml1 + hhdm))[pml1_entry];
}

void* kmalloc(size_t bytes) {
    if(bytes <= 4096-514) return heap_allocate(bytes);
    else if(bytes <= 4096) return page_allocate(0);
    return 0;
}

void kdemalloc(void* data) {
    if((size_t)data & 0xFFF) heap_deallocate(data);
    else page_deallocate(data);
}

size_t new_pmap() {
    uint64_t ret=0;
    if(!(ret=alloc_page())) return 0;
    memset((void*)(ret+hhdm),0,4096);
    uint64_t* retp = (uint64_t*)(ret+get_hhdm());
    uint64_t* pmap = (uint64_t*)(get_pmap()+get_hhdm());
    //retp[0] = pmap[0];
    //retp[4] = pmap[4];
    retp[(hhdm>>39)&0x1FF] = pmap[(hhdm>>39)&0x1FF];
    retp[511] = pmap[511];
    return ret;
}

size_t get_hhdm() {
    return hhdm;
}