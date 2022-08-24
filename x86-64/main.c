#include <x86-64/cpu.h>
#include <x86-64/mm.h>
#include <x86-64/fb.h>
#include <x86-64/fs.h>
#include <x86-64/task.h>
#include <x86-64/syscall.h>
#include <limine.h>

void _start() {
    init_gdt();
    init_idt();
    init_mm();
    init_fifo();
    init_tar();
    init_task();
    init_syscall();
    init_keyb();
    exec_handler("init");
    fb_draw_rotom_logo(0,0);fb_draw_rotom_text(128,0);
    for(;;) {
        idle();
    }
}