#include <kernel/fb.h>
#include <kernel/fifo.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/keyb.h>
#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/task.h>

extern "C" {

void _start() {
    GdtInitialize();
    IdtInitialize();
    MemPhyInitialize();
    MemBlkInitialize();
    MemHeapInitialize(128);
    TaskInitialize();
    FifoInitialize();
    KeybInitialize();
    printf("Done.\n");
    for(;;) {
        KeybEcho();
        KeIdle();
    }
}

}