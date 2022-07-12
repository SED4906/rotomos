#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/task.h>
#include <kernel/idt.h>

void _start() {
    init_page();
    init_idt();
    init_task();
    for(;;) __asm__ volatile("hlt");
}