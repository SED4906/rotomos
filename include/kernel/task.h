#pragma once
#include <stddef.h>
typedef struct {
    size_t rsp, cr3;
} context;

typedef struct context_list{
    context c;
    struct context_list* next;
} context_list;

extern void context_switch();
extern void context_switch_nosave(size_t rsp, size_t cr3);
context switch_task(size_t rsp, size_t cr3);
void init_task();
void add_task(size_t rsp, size_t cr3);
void exit_task();