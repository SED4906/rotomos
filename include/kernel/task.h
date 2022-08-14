#pragma once
#include <stddef.h>
typedef struct {
    size_t rsp, cr3;
} context;

typedef struct context_list{
    context c;
	int pid;
    struct context_list* next;
} context_list;

extern void context_switch();
extern void context_switch_nosave(size_t rsp, size_t cr3);

//! @brief Stores the arguments in the current task's structure,
//! then traverses to the next element of the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
//! @return A stack pointer and page map.
context switch_task(size_t rsp, size_t cr3);

//! @brief - Sets up a circular list of tasks,
//! - context switches to initialize the first item,
//! - and enables a timer for further pre-emptive switches.
void init_task();

//! @brief Adds a task to the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
void add_task(size_t rsp, size_t cr3);

//! @brief Removes the current task from the list,
//! and switches to the next task up.
//! @return Does not return.
void exit_task();
