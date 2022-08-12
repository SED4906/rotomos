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
    kdemalloc(current);
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
    while(current->next) current = current->next;
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

file_handle* open_fifo(char* name, char mode) {
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
    file_handle* handle = kmalloc(sizeof(file_handle));
    handle->address = (char*)current->data.data;
    handle->fstype = 0;
    while(((fifo*)handle->address)->next) {
        page_deallocate(handle->address);
        handle->address = (char*)((fifo*)handle->address)->next;
    }
    handle->pos = ((fifo*)handle->address)->size;
    current->data.data = (fifo*)handle->address;
    page_allocate(handle->address);
    return handle;
}

void close_fifo(file_handle* handle) {
    fifo* current=(fifo*)handle->address;
    page_deallocate(current);
    kdemalloc(handle);
}