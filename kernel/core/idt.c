#include <kernel/core.h>
#include <kernel/libc.h>
#include <stddef.h>
#define GDT_OFFSET_KERNEL_CODE 0x08
#define IDT_MAX_DESCRIPTORS 256

__attribute__((aligned(0x10))) 
idt_entry idt[256]; // Create an array of IDT entries; aligned for performance
idtr idtr_idt;

void set_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry* descriptor = &idt[vector];
 
    descriptor->isr_low       = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved           = 0;
}

extern void* isr_stub_table[49];

void init_idt() {
    idtr_idt.base = (uintptr_t)&idt[0];
    idtr_idt.limit = (uint16_t)sizeof(idt_entry) * IDT_MAX_DESCRIPTORS - 1;
 
    for (uint8_t vector = 0; vector < 49; vector++) {
        set_idt_entry(vector, isr_stub_table[vector], 0x8E);
    }

    set_idt_entry(32, isr_stub_table[32], 0x8F);

    init_pic(32, 40);
    load_idt(&idtr_idt);
}

void reload_idt() {
    load_idt(&idtr_idt);
}

__attribute__((noreturn))
void exception_err_handler(size_t code) {
    printf("Unhandled Exception: Error Code - %x", code);
    for(;;) hang_forever();
}
__attribute__((noreturn))
void exception_no_err_handler() {
    printf("Unhandled Exception");
    for(;;) hang_forever();
}
__attribute__((noreturn))
void page_fault_handler(size_t error) {
    void* address;
    __asm__ volatile("movq %%cr2, %0":"=r"(address));
    printf("Page fault at %x (%X)", address, error);
    for(;;) hang_forever();
}
__attribute__((noreturn))
void double_fault_handler() {
    printf("Double fault");
    for(;;) hang_forever();
}
void irq_handler() {
    printf("Unknown IRQ recieved\n");
    pic_eoi(16);
}
void pit_handler() {
    pic_eoi(0);
    context_switch();
}
void is_working_handler(uint64_t rsp) {
    printf("%x!",rsp);
    reload_idt();
}