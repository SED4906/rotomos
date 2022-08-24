#pragma once
#include <stddef.h>
typedef struct {
    char* address;
    short fstype;
    size_t size;
    size_t pos;
} file_handle;

typedef struct
{
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
} tar_header;

typedef struct tar_header_list
{
    tar_header* header;
    struct tar_header_list* next;
} tar_header_list;

void init_tar();

file_handle* open_tar(char* path);

size_t read_tar(file_handle* handle, char* str, size_t len);

size_t write_tar(file_handle* handle, char* str, size_t len);

void close_tar(file_handle* handle);

typedef struct fifo {
    size_t size;
    struct fifo* next;
} fifo;

typedef struct {
    char* name;
    fifo* data;
} fifo_header;

typedef struct fifo_header_list
{
    fifo_header data;
    struct fifo_header_list* next;
    struct fifo_header_list* prev;
} fifo_header_list;

void init_fifo();
file_handle* open_fifo(char* name);
size_t read_fifo(file_handle* fd, char* str, size_t len);
size_t write_fifo(file_handle* fd, char* str, size_t len);
void close_fifo(file_handle* fd);
void create_fifo(char* name);
void destroy_fifo(char* name);