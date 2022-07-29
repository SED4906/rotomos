#include <kernel/libc.h>
#include <kernel/core.h>
#include <kernel/keyb.h>

void _start() {
    init_idt();
    init_mm();
    init_task();
    init_keyb();
    for(;;) hang_idle();
}