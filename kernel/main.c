#include <kernel/libc.h>
#include <kernel/core.h>

void _start() {
    init_page();
    init_idt();
    init_task();
    init_keyb();
    for(;;) __asm__ volatile("hlt");
}