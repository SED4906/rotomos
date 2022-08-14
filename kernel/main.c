#include <kernel/cpu.h>
#include <kernel/exec.h>
#include <kernel/fb.h>
#include <kernel/fs.h>
#include <kernel/gdt.h>
#include <kernel/keyb.h>
#include <kernel/idt.h>
#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/mouse.h>
#include <kernel/pci.h>
#include <kernel/syscall.h>
#include <kernel/task.h>
#include <kernel/tty.h>
#include <limine.h>

void _start() {
    init_gdt();
    init_idt();
    init_mm();
    init_pci();
    init_fifo();
    init_tar();
    init_keyb();
    init_tty();
    init_task();
    exec_handler("init");
    fb_draw_rotom_logo(0,0);fb_draw_rotom_text(128,0);
    for(;;) {
        hang_idle();
    }
}