#include <stddef.h>
#include <stdint.h>
#include <x86-64/fs.h>
#include <kernel/libc.h>
#include <kernel/list.h>
#include <x86-64/mm.h>
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
        file_list = heap_allocate(sizeof(tar_header_list));
        memset(file_list, 0, sizeof(tar_header_list));
        file_list->next = next;
        file_list->header = current;
        printf("File: %s\n",current->filename);
        current = (tar_header*)(((size_t)current)+(size&~0x1FF) + 512);
        if(size % 512) current = (tar_header*)(((size_t)current)+512);
    }
}

file_handle* open_tar(char* path) {
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
    file_handle* handle = heap_allocate(sizeof(file_handle));
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
size_t write_tar(file_handle* handle, char* str, size_t len) {
    return 0;
}
#pragma clang diagnostic pop

void close_tar(file_handle* handle) {
    if(!handle) return;
    heap_deallocate(handle);
}

fifo_header_list* fifos;

void init_fifo() {
    fifos = 0;
}

void create_fifo(char* name) {
    fifo_header_list* fifo = heap_allocate(sizeof(fifo_header_list));
    fifo->next = fifos;
    fifo->prev = 0;
    fifo->data.name = name;
    fifo->data.data = page_allocate(0);
    if(fifos) fifos->prev = fifo;
    fifos = fifo;
}

void destroy_fifo(char* name) {
    fifo_header_list* current = fifos;
    while (current) {
        int found=1;
        for(int i=0;i<strlen(name);i++) {
            if(current->data.name[i] != name[i]) {
                found=0;
                current = current->next;
                break;
            }
        }
        if(found) break;
    }
    if(!current) return;
    if(current->next) current->next->prev = current->prev;
    if(current->prev) current->prev->next = current->next;
    page_deallocate(current);
}

size_t read_fifo(file_handle* handle, char* str, size_t len) {
    if(!handle) return 0;
    size_t possible = ((fifo*)handle->address)->size - handle->pos;
    size_t complete = 0;
    size_t remaining = len;
    if(possible == 0) return 0;
    while(possible < remaining) {
        memcpy(str+complete,handle->address + sizeof(fifo) + handle->pos,possible);
        handle->pos += possible;
        complete += possible;
        remaining -= possible;
        if(((fifo*)handle->address)->size < 4096-sizeof(fifo)) continue;
        if(!((fifo*)handle->address)->next) return complete;
        page_deallocate(handle->address);
        handle->address=(char*)(((fifo*)handle->address)->next);
        page_allocate(handle->address);
        handle->pos = 0;
        possible = ((fifo*)handle->address)->size - handle->pos;
    }
    memcpy(str+complete,handle->address + sizeof(fifo) + handle->pos,remaining);
    handle->pos += remaining;
    return len;
}

size_t write_fifo(file_handle* handle, char* str, size_t len) {
    if(!handle) return 0;
    fifo* current=(fifo*)handle->address;
    current = list_last(current, sizeof(size_t));
    size_t possible = 4096 - (sizeof(fifo) + current->size);
    size_t complete = 0;
    size_t remaining = len;
    while(possible < remaining) {
        memcpy((char*)(current + 1) + current->size,str+complete,possible);
        complete += possible;
        current->size += possible;
        remaining -= possible;
        if(!current->next) {
            current->next = (fifo*)page_allocate(0);
        }
        current = current->next;
        possible = 4096 - (sizeof(fifo) + current->size);
    }
    memcpy((char*)(current + 1) + current->size,str+complete,remaining);
    current->size += remaining;
    return len;
}

file_handle* open_fifo(char* name) {
    if(!name) return 0;
    fifo_header_list* current = fifos;
    while (current) {
        int found=1;
        for(int i=0;i<strlen(name);i++) {
            if(current->data.name[i] != name[i]) {
                found=0;
                current = current->next;
                break;
            }
        }
        if(found) break;
    }
    if(!current) return 0;
    file_handle* handle = heap_allocate(sizeof(file_handle));
    handle->address = (char*)current->data.data;
    handle->fstype = 0;
    handle->address = (char*)list_last_callback((void*)handle->address, sizeof(size_t), &page_deallocate);
    handle->pos = ((fifo*)handle->address)->size;
    current->data.data = (fifo*)handle->address;
    return handle;
}

void close_fifo(file_handle* handle) {
    fifo* current=(fifo*)handle->address;
    page_deallocate(current);
    heap_deallocate(handle);
}