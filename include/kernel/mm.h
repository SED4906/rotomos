#pragma once
#include <stddef.h>

struct PhyPage {
    size_t address;
    int refs;
};

typedef char Heap[448];

void MemPhyInitialize();
void MemPhyLink(size_t address);
size_t MemPhyUnlink();
size_t MemDirectOffset();
void MemBlkInitialize();
size_t MemBlkReclaim(size_t pages);
void MemHeapInitialize(size_t pages);
size_t MemHeapAllocate(size_t bytes);
void MemHeapDeallocate(size_t address);