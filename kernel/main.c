#include <kernel/libc.h>
#include <kernel/core.h>
#include <kernel/exec.h>
#include <kernel/keyb.h>
#include <kernel/fs.h>
#include <kernel/mouse.h>
#include <kernel/tty.h>
#include <limine.h>

/*uint8_t test_binary[48] = {0xBF, 0x01, 0x00, 0x00, 0x00, 0x48, 0xBE, 0x1B, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 
0x8B, 0x14, 0x25, 0x28, 0x00, 0x80, 0x00, 0xCD, 0x30, 0xCD, 0x32, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 
0x2C, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};*/

void _start() {
    init_gdt();
    init_idt();
    init_mm();
    init_pci();
    init_fifo();
    init_tar();
    init_keyb();
    //init_mouse();
    init_tty();
    init_task();
    exec_handler("init");
    fb_draw_rotom_logo(0,0);fb_draw_rotom_text(128,0);
    for(;;) {
        hang_idle();
    }
}