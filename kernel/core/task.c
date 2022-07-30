#include <kernel/core.h>
#include <stddef.h>
context_list* contexts;


context switch_task(size_t rsp, size_t cr3) {
    contexts->c.rsp = rsp;
    contexts->c.cr3 = cr3;
    contexts = contexts->next;
    return contexts->c;
}

void init_task() {
    contexts = (context_list*)kmalloc(sizeof(context_list));
    contexts->next = contexts;
    context_switch();
    init_pit(20);
    pic_clear_mask(0);
}

void add_task(size_t rsp, size_t cr3) {
    context_list* context = (context_list*)kmalloc(sizeof(context_list));
    context->c.rsp = rsp;
    context->c.cr3 = cr3;
    context->next = contexts->next;
    contexts->next = context;
}

void exit_task() {
    context_list* context = contexts;
    while(context->next != contexts);
    context->next = contexts->next;
    kdemalloc(contexts);
    contexts = context->next;
    context_switch_nosave(contexts->c.rsp,contexts->c.cr3);
    for(;;) hang_idle();
}