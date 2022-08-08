#pragma once
#include <stddef.h>
#include <kernel/libc.h>
#include <kernel/list.h>
struct VfsHandle {
    int fd;
    void* node;
    size_t offset;
    virtual int Read(void* buffer, int bytes);
    virtual int Write(void* buffer, int bytes);
    virtual void* Map(void* address, int length, int offset);
    virtual int Unmap(void* address, int length);
    virtual int Seek(int bytes);
    virtual int Tell();
};

struct VfsNode {
    char* name;
    LL<VfsHandle*>* handles;
    VfsNode* parent;
    LL<VfsNode*>* children;
    char* data;
    size_t size;
    virtual VfsHandle* Open();
    virtual void Close(VfsHandle* handle);
};

void VfsInitialize();