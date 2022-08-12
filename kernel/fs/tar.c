#include <kernel/core.h>
#include <kernel/fs.h>
#include <kernel/libc.h>
#include <limine.h>
tar_header_list* file_list;

struct limine_module_request module_request = {
    .id=LIMINE_MODULE_REQUEST,
    .revision = 0, .response = 0
};

uint64_t tar_getsize(const char *in)
{
    uint64_t size = 0;
    uint64_t j;
    uint64_t count = 1;
 
    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);
 
    return size;
}

void init_tar() {
    if(!module_request.response) return;
    file_list = 0;
    tar_header* current = module_request.response->modules[0]->address;
    while (current->filename[0])
    {
        size_t size = tar_getsize(current->size);
        tar_header_list* next = file_list;
        file_list = kmalloc(sizeof(tar_header_list));
        memset(file_list, 0, sizeof(tar_header_list));
        file_list->next = next;
        file_list->header = current;
        printf("Found file: %s\n",current->filename);
        current = (tar_header*)(((size_t)current)+(size&~0x1FF) + 512);
        if(size % 512) current = (tar_header*)(((size_t)current)+512);
    }
}

file_handle* open_tar(char* path, char mode) {
    tar_header_list* current = file_list;
    while (current) {
        int found=1;
        for(int i=0;i<strlen(path);i++) {
            if(current->header->filename[i] != path[i]) {
                found=0;
                current = current->next;
                break;
            }
        }
        if(found) break;
    }
    if(!current) return 0;
    file_handle* handle = kmalloc(sizeof(file_handle));
    handle->fstype = 1;
    handle->address=(char*)(((size_t)current->header)+512);
    handle->pos = 0;
    handle->size = tar_getsize(current->header->size);
    return handle;
}

size_t read_tar(file_handle* handle, char* str, size_t len) {
    if(handle->size-handle->pos > len) {
        memcpy(str,handle->address+handle->pos,len);
        handle->pos += len;
        return len;
    }
    size_t bytes = handle->size-handle->pos;
    memcpy(str, handle->address+handle->pos, bytes);
    handle->pos = handle->size;
    return bytes;
}

size_t write_tar(file_handle* handle, char* str, size_t len) {
    return 0;
}

void close_tar(file_handle* handle) {
    if(!handle) return;
    kdemalloc(handle);
}