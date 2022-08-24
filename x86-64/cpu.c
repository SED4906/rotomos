#include <kernel/libc.h>
#include <x86-64/cpu.h>
#include <stdint.h>
__attribute__((aligned(0x10))) 
idt_entry idt[256];
idtr idtr_idt;
uint64_t gdt[7];
gdtr gdtr_gdt;
tss tss_gdt;

void set_gdt_entry(uint64_t* entry, uint64_t base, uint16_t limit, uint8_t access, uint8_t flags) {
    entry[0] = limit & 0xFFFF;
    entry[0] |= (base & 0xFFFFFF) << 16;
    entry[0] |= (uint64_t)access << 40;
    entry[0] |= (uint64_t)(limit & 0xF0000) << 48;
    entry[0] |= (uint64_t)(flags & 0x0F) << 52;
    entry[0] |= (uint64_t)(base & 0xFF000000) << 56;
    if(!(access & 16)) {
        entry[1] = base >> 32;
    }
}

void set_idt_entry(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry* descriptor = &idt[vector];
 
    descriptor->isr_low       = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs             = 0x08;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved           = 0;
}

extern void pic_clear_mask(char irq);
void install_irq_handler(uint8_t irq, void* isr, uint8_t flags) {
    set_idt_entry(32+irq, isr, flags);
    pic_clear_mask(irq);
}

void init_gdt() {
    set_gdt_entry(&gdt[0], 0, 0, 0, 0);
    set_gdt_entry(&gdt[1], 0, 0, 0x9A, 0xA);
    set_gdt_entry(&gdt[2], 0, 0, 0x92, 0xC);
    set_gdt_entry(&gdt[3], 0, 0, 0xFA, 0xA);
    set_gdt_entry(&gdt[4], 0, 0, 0xF2, 0xC);
    set_gdt_entry(&gdt[5], (uint64_t)&tss_gdt, sizeof(tss), 0x89, 0x0);
    memset(&tss_gdt, 0, sizeof(tss));
    gdtr_gdt.limit = 0x37;
    gdtr_gdt.base = (uint64_t)gdt;

    load_gdt(&gdtr_gdt);
    load_tss();
    printf("GDT+TSS\n");
}

extern void* isr_stub_table[48];
extern void init_pic(char m, char s);
void init_idt() {
    idtr_idt.base = (uintptr_t)&idt[0];
    idtr_idt.limit = (uint16_t)sizeof(idt_entry) * 256 - 1;
 
    for (uint8_t vector = 0; vector < 48; vector++) {
        set_idt_entry(vector, isr_stub_table[vector], 0x8E);
    }
    
    init_pic(32, 40);
    load_idt(&idtr_idt);
    printf("IDT\n");
}

void exception_no_err_handler() {
    printf("<fatal exception>");
    for(;;) die();
}

void exception_err_handler(uint64_t code) {
    printf("<fatal exception:%X>", code);
    for(;;) die();
}

void exception_pf_handler(uint64_t code, uint64_t addr) {
    printf("<page fault:%X:%X>", code, addr);
    for(;;) die();
}