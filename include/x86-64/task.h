#pragma once
#include <stddef.h>

typedef struct {
    size_t rsp, cr3;
} context;

typedef struct task_list {
    context c;
    int pid;
    struct task_list* next;
} task_list;

void init_task();

int add_task(size_t rsp, size_t cr3);

void exit_task();

int bin_exec(size_t file, size_t len);