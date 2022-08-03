#include <kernel/libc.h>
#include <kernel/core.h>
#include <kernel/exec.h>
#include <kernel/keyb.h>

uint8_t test_binary[5] = {0xCD, 0x30, 0xF4, 0xEB, 0xFB};

void _start() {
    init_gdt();
    init_idt();
    init_mm();
    init_task();
    init_keyb();
    fb_draw_rotom_logo(0,0);fb_draw_rotom_text(128,0);
    for(;;) {
        hang_idle();
    }
}