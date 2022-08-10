#pragma once
#include <kernel/libc.h>
#include <kernel/list.h>
#include <stddef.h>

struct FifoHandle
{
    int offset;
    LL<int>* current;
};

int FifoRead(FifoHandle* handle, void* buffer, size_t bytes);
int FifoWrite(FifoHandle* handle, void* buffer, size_t bytes);

struct Fifo {
    char* name;
    size_t size;
    LL<int>* data;
    LL<FifoHandle>* handles;
};

Fifo* CreateFifo(char* name);
void DestroyFifo(Fifo* fifo);

FifoHandle* FifoOpen(Fifo* fifo);
void FifoClose(FifoHandle* handle);

void FifoInitialize();
Fifo* FifoSearch(char* name);