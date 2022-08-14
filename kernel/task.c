#include <kernel/cpu.h>
#include <kernel/fs.h>
#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/pic.h>
#include <kernel/task.h>
#include <stddef.h>
context_list* contexts;
int next_pid=0;

file_handle* tty_handle;

context switch_task(size_t rsp, size_t cr3) {
    contexts->c.rsp = rsp;
    contexts->c.cr3 = cr3;
    char c=0;
    while(read_fifo(tty_handle,&c,1) && c) printf("%c",c);
    contexts = contexts->next;
    return contexts->c;
}

void init_task() {
    tty_handle = open_fifo("tty",'r');
    contexts = (context_list*)kmalloc(sizeof(context_list));
    contexts->next = contexts;
    contexts->pid = next_pid++;
    init_pit(69);
    pic_clear_mask(0);
    context_switch();
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