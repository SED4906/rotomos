#include <kernel/core.h>
#include <stddef.h>
context_list* contexts;
int next_pid=0;

context switch_task(size_t rsp, size_t cr3) {
    contexts->c.rsp = rsp;
    contexts->c.cr3 = cr3;
    contexts = contexts->next;
    return contexts->c;
}

void init_task() {
    contexts = (context_list*)kmalloc(sizeof(context_list));
    contexts->next = contexts;
    contexts->pid = next_pid++;
    init_pit(69);
    pic_clear_mask(0);
    hang_idle();
}

void add_task(size_t rsp, size_t cr3) {
    context_list* context = (context_list*)kmalloc(sizeof(context_list));
    context->c.rsp = rsp;
    context->c.cr3 = cr3;
    context->pid = next_pid++;
    context->next = contexts->next;
    contexts->next = context;
}

void exit_task() {
    context_list* context = contexts;
    while(context->next != contexts) context = context->next;
    context->next = contexts->next;
    kdemalloc(contexts);
    contexts = context->next;
    context_switch_nosave(contexts->c.rsp,contexts->c.cr3);
    for(;;) hang_idle();
}