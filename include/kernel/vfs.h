#pragma once
#include <stddef.h>
#include <kernel/libc.h>
#include <kernel/list.h>

struct VfsHandle {
    size_t size;
    size_t offset;
    virtual int Read(void* buffer, size_t bytes);
    virtual int Write(void* buffer, size_t bytes);
    virtual ~VfsHandle();
};

struct VfsAppend : VfsHandle {
    LL<size_t>** data_s;
    LL<size_t>** data_e;
    int Read(void* buffer, size_t bytes) override;
    int Write(void* buffer, size_t bytes) override;
    virtual ~VfsAppend();
};

struct VfsNode {
    char* name;
    VfsNode* parent;
    LL<VfsNode*>* children;
    LL<VfsNode*>* Children();
    VfsNode* Parent();
    virtual VfsHandle* Open();
    virtual void Close(VfsHandle* hnadle);
    virtual ~VfsNode();
};

struct VfsAppendNode : VfsNode {
    LL<size_t>* data_s;
    LL<size_t>* data_e;
    virtual VfsHandle* Open() override;
    virtual void Close(VfsHandle* handle) override;
    virtual ~VfsAppendNode();
};

void VfsInitialize();