#include <kernel/libc.h>
#include <kernel/core.h>
#include <kernel/exec.h>
#include <kernel/keyb.h>

uint8_t test_binary[4] = {0xCD, 0x20, 0xEB, 0xFC};

void _start() {
    init_idt();
    init_mm();
    init_task();
    init_keyb();
    flatbinary_exec((size_t)test_binary,4);
    flatbinary_exec((size_t)test_binary,4);
    printf("Is this thing on?\n");
    uint64_t de_buggy = 0;
    for(;;) {
        printf("%d", de_buggy++);
        hang_idle();
    }
}