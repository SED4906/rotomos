#include <kernel/core.h>
#include <stddef.h>
context_list* contexts;

//! @brief Stores the arguments in the current task's structure,
//! then traverses to the next element of the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
//! @return A stack pointer and page map.
context switch_task(size_t rsp, size_t cr3) {
    contexts->c.rsp = rsp;
    contexts->c.cr3 = cr3;
    contexts = contexts->next;
    return contexts->c;
}

//! @brief - Sets up a circular list of tasks,
//! - context switches to initialize the first item,
//! - and enables a timer for further pre-emptive switches.
void init_task() {
    contexts = (context_list*)kmalloc(sizeof(context_list));
    contexts->next = contexts;
    context_switch();
    init_pit(20);
    pic_clear_mask(0);
}

//! @brief Adds a task to the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
void add_task(size_t rsp, size_t cr3) {
    context_list* context = (context_list*)kmalloc(sizeof(context_list));
    context->c.rsp = rsp;
    context->c.cr3 = cr3;
    context->next = contexts->next;
    contexts->next = context;
}

//! @brief Removes the current task from the list,
//! and switches to the next task up.
//! @return Does not return.
void exit_task() {
    context_list* context = contexts;
    while(context->next != contexts);
    context->next = contexts->next;
    kdemalloc(contexts);
    contexts = context->next;
    context_switch_nosave(contexts->c.rsp,contexts->c.cr3);
    for(;;) hang_idle();
}