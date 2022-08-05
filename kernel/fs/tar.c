#include <kernel/core.h>
#include <kernel/fs.h>
#include <kernel/libc.h>
#include <limine.h>
tar_header_list* file_list;
opened_tar_list* open_file_list;
int next_fd=2;

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
    open_file_list=0;
}

int open_tar(char* path, char mode) {
    tar_header_list* current = file_list;
    while (current) {
        int found=1;
        for(size_t i=0;i<strlen(path);i++) {
            if(current->header->filename[i] != path[i]) {
                found=0;
                current = current->next;
                break;
            }
        }
        if(found) break;
    }
    if(!current) return -1;
    opened_tar_list* next = open_file_list;
    open_file_list = kmalloc(sizeof(opened_tar_list));
    open_file_list->next=next;
    open_file_list->address=(char*)(((size_t)current->header)+512);
    open_file_list->fd = next_fd++;
    open_file_list->pos = 0;
    open_file_list->size = tar_getsize(current->header->size);
    return open_file_list->fd;
}

size_t read_tar(int fd, char* str, size_t len) {
    opened_tar_list* current = open_file_list;
    while (current && current->fd != fd) current=current->next;
    if(!current || current->fd != fd) return 0;
    if(current->size-current->pos > len) {
        memcpy(str,current->address+current->pos,len);
        current->pos += len;
        return len;
    }
    size_t bytes = current->size-current->pos;
    memcpy(str, current->address+current->pos, bytes);
    current->pos = current->size;
    return bytes;
}

size_t write_tar(int fd, char* str, size_t len) {
    return 0;
}

void close_tar(int fd) {
    opened_tar_list* prev = open_file_list;
    opened_tar_list* current = open_file_list;
    if(!current) return;
    if(current && current->fd == fd) {
        opened_tar_list* next = current->next;
        kdemalloc(current);
        open_file_list=next;
        return;
    }
    while (prev->next && prev->next->fd != fd)
    {
        prev=prev->next;
    }
    current=prev->next;
    if(!current || current->fd != fd) return;
    prev->next = current->next;
    kdemalloc(current);
}