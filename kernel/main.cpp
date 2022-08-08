#include <kernel/fb.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/libc.h>
#include <kernel/mm.h>
#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/vfs.h>

extern "C" {

void _start() {
    GdtInitialize();
    IdtInitialize();
    MemPhyInitialize();
    MemBlkInitialize();
    MemHeapInitialize(128);
    TaskInitialize();
    VfsInitialize();
    printf("Done.\n");
    for(;;) KeIdle();
}

}