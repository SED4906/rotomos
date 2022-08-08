#include <kernel/libc.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/vfs.h>

VfsNode* vfs_root;
int next_fd=0;

void VfsInitialize() {
    vfs_root = new VfsNode;
    vfs_root->children = 0;
    vfs_root->parent = 0;
    vfs_root->name = "";
}

LL<VfsNode*>* VfsNode::Children() {
    return children;
}

VfsNode* VfsNode::Parent() {
    return parent;
}

VfsHandle* VfsNode::Open() {
    VfsAppend* handle = new VfsAppend;
    handle->offset=0;handle->size=0;
    return handle;
}

void VfsNode::Close(VfsHandle* handle) {
    delete handle;
}

int VfsHandle::Read(void* buffer, size_t bytes) {
    return -1;
}

int VfsHandle::Write(void* buffer, size_t bytes) {
    return -1;
}

int VfsAppend::Read(void* buffer, size_t bytes) {
    size_t pos=0;
    LL<size_t>* current=*data_s;
    while (pos<offset)
    {
        pos += current->data;
        current=current->next;
    }
    if(pos>offset && current) current=current->prev;
    size_t remaining = bytes;
    size_t complete = 0;
    while(remaining && current) {
        size_t change = offset - pos;
        memcpy((char*)buffer + complete,(char*)(current+1)+change,current->data - change);
        remaining -= current->data - change;
        complete += current->data - change;
        pos += current->data;
        offset = pos;
        current=current->next;
    }
    return (int)complete;
}

int VfsAppend::Write(void* buffer, size_t bytes) {
    size_t pos=size;
    LL<size_t>* current=*data_e;
    size_t capacity = 4096 - sizeof(LL<size_t>);
    size_t remaining = bytes;
    size_t complete = 0;
    while(remaining && current) {
        size_t available = capacity - current->data;
        size_t possible = available;
        if(available > remaining) possible = remaining;
        memcpy((char*)(current+1)+current->data,(char*)buffer + complete,possible);
        remaining -= possible;
        complete += possible;
        pos += current->data;
        size += possible;
        LL<size_t>* nextpage = (LL<size_t>*)(MemBlkAllocate(0)+MemDirectOffset());
        if(!nextpage) break;
        ListLink(*data_e,nextpage);
        *data_e=nextpage;
        current=*data_e;
    }
    return (int)complete;
}

VfsHandle* VfsAppendNode::Open() {
    VfsAppend* handle = new VfsAppend;
    handle->offset=0;handle->size=0;
    handle->data_s=&data_s;
    handle->data_e=&data_e;
}

void VfsAppendNode::Close(VfsHandle* handle) {
    delete handle;
}

VfsAppendNode::VfsAppendNode() {
    data_s=(LL<size_t>*)(MemBlkAllocate(0)+MemDirectOffset());
    data_e=data_s;
}