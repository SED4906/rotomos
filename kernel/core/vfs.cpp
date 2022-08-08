#include <kernel/libc.h>
#include <kernel/list.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/vfs.h>

VfsNode* vfs_root;
int next_fd=0;

void VfsInitialize() {
    vfs_root = new VfsNode;
    vfs_root->size = 4096;
    vfs_root->data = (char*)MemBlkAllocate(0);
    if(!vfs_root->data) KePanic("<oom:vfs>");
}

int VfsHandle::Read(void* buffer, int bytes) {
    char* data = ((VfsNode*)node)->data;
    size_t size = ((VfsNode*)node)->size;
    if(bytes<=0 || size-offset <= 0) return 0;
    if(size-offset < (size_t)bytes) {
        size_t copied=size-offset;
        memcpy(buffer,data+offset,size-offset);
        offset=size;
        return (int)copied;
    }
    memcpy(buffer, data+offset, bytes);
    offset+=bytes;
    return bytes;
}

int VfsHandle::Write(void* buffer, int bytes) {
    char* data = ((VfsNode*)node)->data;
    if(bytes<=0) return 0;
    memcpy(data+offset,buffer,bytes);
    offset+=bytes;
    return bytes;
}

void* VfsHandle::Map(void* address, int length, int offset) {
    char* data = ((VfsNode*)node)->data;
    size_t page=0;
    if((size_t)address >= (size_t)256<<39) return 0;
    if((size_t)address & 0xFFF) return 0;
    if(length == 4096 && !(offset & 0xFFF)) {
        page = MemBlkAllocate((size_t)data+offset);
    } else if (length == 4096) {
        page = MemBlkAllocate(0);
    } else return 0;
    if(!page) return 0;
    if(!MemMapPage(MemCurrentPagemap(),(size_t)address,(size_t)page,7)) return 0;
    return address;
}

int VfsHandle::Unmap(void* address, int length) {
    if(!address) return -1;
    if((size_t)address >= (size_t)256<<39) return -1;
    if((size_t)address & 0xFFF) return -1;
    if(length != 4096) return -1;
    size_t page=0;
    if(!(page = MemUnmapPage(MemCurrentPagemap(),(size_t)address))) return -1;
    MemBlkDeallocate(page);
    return 0;
}

int VfsHandle::Seek(int bytes) {
    size_t size = ((VfsNode*)node)->size;
    int ssize = (int)size;
    int soffset = offset;
    int temp=soffset;
    soffset += bytes;
    if(soffset<0) {offset=0;return -temp;}
    else if(soffset>ssize) {offset=size;return ssize-temp;}
    else {offset = (size_t)soffset;return bytes;}
}

int VfsHandle::Tell() {
    return (int)offset;
}

VfsHandle* VfsNode::Open() {
    VfsHandle* handle = new VfsHandle;
    handle->node = this;
    handle->fd = next_fd++;
    return handle;
}

void VfsNode::Close(VfsHandle* handle) {
    delete handle;
}