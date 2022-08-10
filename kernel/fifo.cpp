#include <kernel/fifo.h>
#include <kernel/libc.h>
#include <kernel/mm.h>

LL<Fifo>* fifos;

Fifo* CreateFifo(char* name) {
    LL<Fifo>* fifo = new LL<Fifo>;
    fifo->next=0;fifo->prev=0;
    fifo->data.name=name;
    fifo->data.data=(LL<int>*)MemBlkAllocate(0)+MemDirectOffset();
    memset(fifo->data.data,0,4096);
    ListLink(fifo,fifos);
    fifos = fifo;
    return (Fifo*)fifo;
}

void DestroyFifo(Fifo* fifo) {
    LL<FifoHandle>* handle = fifo->handles;
    while(handle) {
        delete handle;
        handle=handle->next;
    }
    LL<int>* current = fifo->data;
    while (current) {
        MemBlkDeallocate((size_t)(current)-MemDirectOffset());
        current=current->next;
    }
    delete ListUnlink((LL<Fifo>*)fifo);
}

FifoHandle* FifoOpen(Fifo* fifo) {
    LL<FifoHandle>* handle = new LL<FifoHandle>;
    LL<int>* current=fifo->data;
    while(current->next) current=current->next;
    handle->data.current=current;
    handle->data.offset=current->data;
    return (FifoHandle*)handle;
}

void FifoClose(FifoHandle* handle) {
    delete ListUnlink((LL<FifoHandle>*)handle);
}

int FifoRead(FifoHandle* handle, void* buffer, size_t bytes) {
    if(!bytes) return 0;
    printf("r");
    int possible = handle->current->data - handle->offset;
    size_t complete=0;
    if(!buffer) {
        while(possible < bytes) {
            complete += possible;
            handle->offset = handle->current->data;
            if(!handle->current->next) {
                return complete;
            }
            handle->offset = 0;
            handle->current = handle->current->next;
            possible = handle->current->data - handle->offset;
        }
        handle->offset += bytes;
        printf("R?");
        return bytes;
    }
    char* buf = (char*)buffer;
    while(possible < bytes) {
        int possible = handle->current->data - handle->offset;
        memcpy(buf+complete,handle->current+1,possible);
        complete+=possible;
        handle->offset = handle->current->data;
        if(!handle->current->next) {
            printf("R.");
            return possible;
        }
        MemBlkDeallocate((size_t)(handle->current)-MemDirectOffset());
        handle->current = handle->current->next;
        MemBlkAllocate((size_t)(handle->current)-MemDirectOffset());
        handle->offset=0;
    }
    memcpy(buf+complete,handle->current+1,bytes);
    handle->offset += bytes;
    printf("R");
    return bytes;
}

int FifoWrite(FifoHandle* handle, void* buffer, size_t bytes) {
    printf("w");
    if(!bytes || !buffer) return 0;
    LL<int>* current = handle->current;
    while(current->next) current=current->next;
    int* size=&current->data;
    int possible = 4096 - (sizeof(LL<int>) + *size);
    size_t remaining=bytes;
    size_t complete=0;
    char* buf = (char*)buffer;
    while(possible < remaining) {
        char* out=(char*)(current+1);
        memcpy(out+*size,buf+complete,possible);
        complete += possible;
        remaining -= possible;
        current->data = 4096 - sizeof(LL<int>);
        LL<int>* next = (LL<int>*)(MemBlkAllocate(0)+MemDirectOffset());
        if(!next) {
            printf("W!");
            return complete;
        }
        next->data=0;next->next=0;next->prev=0;
        ListLink(current,next);
        current=next;
        size=&current->data;
        possible = 4096 - sizeof(LL<int>);
    }
    char* out=(char*)(current+1);
    memcpy(out+*size,buf+complete,remaining);
    current->data += remaining;
    printf("W");
    return bytes;
}

void FifoInitialize() {
    fifos = 0;
}

Fifo* FifoSearch(char* name) {
    LL<Fifo>* fifo = fifos;
    while (fifo) {
        for(int i=0;i<strlen(name);i++) {
            if(fifo->data.name[i] != name[i]) {
                fifo = fifo->next;
                break;
            }
            return (Fifo*)fifo;
        }
    }
    return 0;
}