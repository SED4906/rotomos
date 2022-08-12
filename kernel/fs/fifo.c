#include <kernel/core.h>
#include <kernel/fs.h>
#include <kernel/libc.h>

fifo_header_list* fifos;

void init_fifo() {
    fifos = 0;
}

void create_fifo(char* name) {
    fifo_header_list* fifo = kmalloc(sizeof(fifo_header_list));
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
        for(size_t i=0;i<strlen(name);i++) {
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
    kdemalloc(current);
}

size_t read_fifo(file_handle* handle, char* str, size_t len) {
    if(!handle) return 0;
    size_t possible = *(size_t*)(handle->address) - handle->pos;
    size_t complete = 0;
    size_t remaining = len;
    while(possible < remaining) {
        memcpy(str+complete,handle->address + sizeof(size_t)*2 + handle->pos,possible);
        handle->pos += possible;
        complete += possible;
        remaining -= possible;
        if(!*(size_t*)(handle->address + sizeof(size_t))) return complete;
        handle->address=(char*)*(size_t*)(handle->address + sizeof(size_t));
        handle->pos = 0;
        possible = *(size_t*)(handle->address) - handle->pos;
    }
    memcpy(str+complete,handle->address + sizeof(size_t)*2 + handle->pos,remaining);
    handle->pos += remaining;
    return len;
}

size_t write_fifo(file_handle* handle, char* str, size_t len) {
    if(!handle) return 0;
    char* current=handle->address;
    while(*(size_t*)(current + sizeof(size_t))) current = (char*)*(size_t*)(current + sizeof(size_t));
    size_t possible = 4096 - (sizeof(size_t)*2 + *(size_t*)(current));
    size_t complete = 0;
    size_t remaining = len;
    while(possible < remaining) {
        memcpy(current + sizeof(size_t)*2 + *(size_t*)(current),str+complete,possible);
        complete += possible;
        *(size_t*)(current) += possible;
        remaining -= possible;
        if(!*(size_t*)(current + sizeof(size_t))) {
            *(size_t*)(current + sizeof(size_t)) = page_allocate(0);
        }
        current = (char*)*(size_t*)(current + sizeof(size_t));
        possible = 4096 - (sizeof(size_t)*2 + *(size_t*)(current));
    }
    memcpy(current + sizeof(size_t)*2 + *(size_t*)(current),str+complete,remaining);
    *(size_t*)(current) += remaining;
    return len;
}

file_handle* open_fifo(char* name, char mode) {
    if(!name) return 0;
    fifo_header_list* current = fifos;
    while (current) {
        int found=1;
        for(size_t i=0;i<strlen(name);i++) {
            if(current->data.name[i] != name[i]) {
                found=0;
                current = current->next;
                break;
            }
        }
        if(found) break;
    }
    if(!current) return 0;
    file_handle* handle = kmalloc(sizeof(file_handle));
    handle->address = current->data.data;
    handle->fstype = 0;
    while(*(size_t*)(handle->address + sizeof(size_t))) handle->address = (char*)*(size_t*)(handle->address + sizeof(size_t));
    handle->pos = *(size_t*)(handle->address);
    return handle;
}

void close_fifo(file_handle* handle) {
    kdemalloc(handle);
}