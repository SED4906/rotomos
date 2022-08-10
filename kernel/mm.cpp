#include <kernel/libc.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <limine.h>

#include <kernel/fb.h>

struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0, .response = 0
};

struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0, .response = 0
};

LL<void*>* freelist;
LL<PhyPage>* pagelist;
LL<Heap>* heaplist;
size_t hhdm;

void MemPhyInitialize() {
    if(!memmap_request.response) KePanic("memmap");
    if(!hhdm_request.response) KePanic("hhdm");
    hhdm = hhdm_request.response->offset;

    for(size_t i=0;i<memmap_request.response->entry_count;i++) {
        struct limine_memmap_entry* entry=memmap_request.response->entries[i];
        printf("%X: %x->%x %X\n", i, entry->base, entry->base + entry->length, entry->type);
        if(entry->type != LIMINE_MEMMAP_USABLE) continue;
        for(size_t j=0;j<entry->length;j+=4096) MemPhyLink(entry->base + j);
    }
}

void MemPhyLink(size_t address) {
    if(!address) return;
    LL<void*>* entry = (LL<void*>*)((address &~ 0xFFF) + hhdm);
    entry->data = (void*)address;
    entry->prev = 0; entry->next = 0;
    ListLink(entry, freelist);
    freelist = entry;
}

size_t MemPhyUnlink() {
    if(!freelist) return 0;
    LL<void*>* ret = (LL<void*>*)freelist;
    freelist=freelist->next;
    return (size_t)ListUnlink(ret)->data;
}

size_t MemDirectOffset() {
    return hhdm;
}

void MemBlkInitialize() {
    bool pages_remain = true;
    while(pages_remain) {
        LL<PhyPage>* pagelist_blk = (LL<PhyPage>*)(MemPhyUnlink() + hhdm);
        if(!pagelist_blk) {
            pages_remain = false;
            break;
        }
        size_t packing_limit = 4096 / sizeof(LL<PhyPage>);
        for(size_t entry=0; entry < packing_limit; entry++) {
            size_t address = MemPhyUnlink();
            if(!address) {
                pages_remain = false;
                break;
            }
            pagelist_blk[entry].data.address = address;
            pagelist_blk[entry].data.refs = 0;
            pagelist_blk[entry].next = 0;pagelist_blk[entry].prev = 0;
            ListLink(&pagelist_blk[entry],pagelist);
            pagelist = &pagelist_blk[entry];
        }
    }
}

size_t MemBlkAllocate(size_t address) {
    LL<PhyPage>* current = pagelist;
    if(!address) {
        while(current && current->data.refs) current = current->next;
        if(!current) return 0;
        current->data.refs++;
        return current->data.address;
    }

    while (current && current->data.address != address) current = current->next;
    if(!current) return 0;
    current->data.refs++;
    return current->data.address;
}

void MemBlkDeallocate(size_t address) {
    LL<PhyPage>* current = pagelist;
    if(!address) return;
    while (current && current->data.address != address) current = current->next;
    if(!current) return;
    if(!current->data.refs) return;
    current->data.refs--;
}

size_t MemBlkReclaim(size_t pages) {
    LL<PhyPage>* pagelist_cur = pagelist;
    size_t reclaimed = 0;
    while (pagelist_cur && reclaimed < pages)
    {
        if(pagelist_cur->data.address && !pagelist_cur->data.refs) {
            MemPhyLink(pagelist_cur->data.address);
            reclaimed++;
        }
        pagelist_cur = pagelist_cur->next;
    }
    return reclaimed;
}

void MemHeapInitialize(size_t pages) {
    size_t actual_pages = MemBlkReclaim(pages);
    for(size_t page=0;page<actual_pages;page++) {
        LL<Heap>* heap = (LL<Heap>*)((size_t)(MemPhyUnlink())+hhdm);
        heap->next=0;heap->prev=0;
        for (size_t i = 0; i < sizeof(Heap); i++) {
            heap->data[i]=0;
        }
        ListLink(heap, heaplist);
        heaplist = heap;
    }
}

size_t MemHeapAllocate(size_t bytes) {
    LL<Heap>* current = heaplist;
    while(current) {
        size_t run=0;
        size_t pos=0;
        for (size_t i = 0; i < sizeof(Heap); i++) {
            for (size_t j = 0; j < 8; j++)
            {
                if(current->data[i] & (1<<j)) {
                    run = 0;
                    pos=i*8+j;
                    continue;
                }
                run++;
                if(run == bytes + 2) break;
            }
            if(run == bytes + 2) break;
        }
        if(run < bytes + 2) {
            current=current->next;
            continue;
        }
        for(size_t bit = pos+1; bit < pos+bytes+1; bit++) {
            current->data[bit >> 3] |= 1 << (bit&7);
        }
        return ((size_t)current->data) + 512 + pos + 1;
    }
    return 0;
}

void MemHeapDeallocate(size_t address) {
    LL<Heap>* current = (LL<Heap>*)(address &~ 0xFFF);
    size_t pos = (address & 0xFFF);
    for(size_t bit = pos-512; current->data[bit >> 3] & (1 << bit&7); bit++) {
        current->data[bit >> 3] ^= 1 << (bit&7);
    }
}

size_t MemMapPageStep(size_t pmap, size_t entry, bool create) {
    size_t* pmap_offset = (size_t*)(pmap+hhdm);
    if(pmap_offset[entry] & 1) return pmap_offset[entry] &~ 0xFFF;
    if(!create) return 0;
    size_t ret = MemBlkAllocate(0);
    if(!ret) return 0;
    memset((void*)(ret+hhdm),0,4096);
    pmap_offset[entry] = ret | 7;
    return ret;
}

size_t MemMapPage(size_t pmap, size_t vaddr, size_t paddr, size_t flags) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(!(pml3 = MemMapPageStep(pmap, pml4_entry, true))) return 0;
    if(!(pml2 = MemMapPageStep(pml3, pml3_entry, true))) return 0;
    if(!(pml1 = MemMapPageStep(pml2, pml2_entry, true))) return 0;
    pml1 += hhdm;
    ((size_t*)(pml1))[pml1_entry] = paddr | flags;
    return vaddr;
}

size_t MemCheckPage(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(!(pml3 = MemMapPageStep(pmap, pml4_entry, false))) return 0;
    if(!(pml2 = MemMapPageStep(pml3, pml3_entry, false))) return 0;
    if(!(pml1 = MemMapPageStep(pml2, pml2_entry, false))) return 0;
    pml1 += hhdm;
    return ((size_t*)(pml1))[pml1_entry];
}

size_t MemUnmapPage(size_t pmap, size_t vaddr) {
    size_t pml4_entry = (vaddr & ((size_t)0x1ff << 39)) >> 39;
    size_t pml3_entry = (vaddr & ((size_t)0x1ff << 30)) >> 30;
    size_t pml2_entry = (vaddr & ((size_t)0x1ff << 21)) >> 21;
    size_t pml1_entry = (vaddr & ((size_t)0x1ff << 12)) >> 12;

    size_t pml3, pml2, pml1;
    if(!(pml3 = MemMapPageStep(pmap, pml4_entry, false))) return 0;
    if(!(pml2 = MemMapPageStep(pml3, pml3_entry, false))) return 0;
    if(!(pml1 = MemMapPageStep(pml2, pml2_entry, false))) return 0;
    pml1 += hhdm;
    ((size_t*)(pml1))[pml1_entry] &=~ 1;
    return ((size_t*)(pml1))[pml1_entry];
}