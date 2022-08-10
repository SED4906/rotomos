#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/panic.h>

void* operator new(size_t bytes) {
    void* ret=0;
    if(bytes <= 3582) ret=(void*)MemHeapAllocate(bytes);
    if(!ret && bytes<=4096) ret=(void*)MemBlkAllocate(0);
    if(!ret) KePanic("<oom>");
    memset(ret, 0, bytes);
    return ret;
}

void* operator new[](size_t bytes) {
    void* ret=0;
    if(bytes <= 3582) ret=(void*)MemHeapAllocate(bytes);
    if(!ret && bytes<=4096) ret=(void*)MemBlkAllocate(0);
    if(!ret) KePanic("<oom>");
    memset(ret, 0, bytes);
    return ret;
}

void operator delete(void* data) {
    if((size_t)data & 0xFFF) MemHeapDeallocate((size_t)data);
    else MemBlkDeallocate((size_t)data);
}

void operator delete[](void* data) {
    if((size_t)data & 0xFFF) MemHeapDeallocate((size_t)data);
    else MemBlkDeallocate((size_t)data);
}

extern "C" void __cxa_pure_virtual()
{
    // Do nothing or print an error message.
}