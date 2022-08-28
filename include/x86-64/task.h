#pragma once
#include <stddef.h>

typedef struct {
    size_t rsp, cr3;
} context;

typedef struct task_list {
    context c;
    size_t r[16];
    int pid;
    struct task_list* next;
} task_list;

void init_task();

int add_task(size_t rsp, size_t cr3, size_t* r);

void exit_task();

int bin_exec(size_t file, size_t len, char* cmdline);