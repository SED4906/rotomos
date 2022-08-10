#include <kernel/cpu.h>
#include <kernel/idt.h>
#include <kernel/libc.h>
#include <stddef.h>
#include <stdint.h>
#define GDT_OFFSET_KERNEL_CODE 0x08
#define IDT_MAX_DESCRIPTORS 256

__attribute__((aligned(0x10))) 
idt_entry idt[256]; // Create an array of IDT entries; aligned for performance
idtr idtr_idt;

void IdtSet(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry* descriptor = &idt[vector];
 
    descriptor->isr_low       = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs             = GDT_OFFSET_KERNEL_CODE;
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid       = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high      = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved           = 0;
}

extern void* isr_stub_table[48];

void IdtInitialize() {
    idtr_idt.base = (uintptr_t)&idt[0];
    idtr_idt.limit = (uint16_t)sizeof(idt_entry) * IDT_MAX_DESCRIPTORS - 1;
 
    for (uint8_t vector = 0; vector < 48; vector++) {
        IdtSet(vector, isr_stub_table[vector], 0x8E);
    }

    IdtSet(32, isr_stub_table[32], 0x8F);

    PicInitialize(32, 40);
    IdtLoad(&idtr_idt);
}

void PicSet(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

void PicClear(unsigned char IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}

void PicEOI(unsigned char irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
 
	outb(PIC1_COMMAND,PIC_EOI);
}

void PicInitialize(int offset1, int offset2)
{
	unsigned char a1, a2;
 
	a1 = inb(PIC1_DATA);                        // save masks
	a2 = inb(PIC2_DATA);
 
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
	outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
	outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	outb(PIC1_DATA, ICW4_8086);
	outb(PIC2_DATA, ICW4_8086);
	outb(PIC1_DATA, a1);   // restore saved masks.
	outb(PIC2_DATA, a2);
}

void PitInitialize(size_t frequency) {
    size_t reload = 1193182 / frequency;
    outb(0x43, 0x36);
    outb(0x40, reload&0xFF);
    outb(0x40, (reload>>8)&0xFF);
    printf("Timer frequency set to %d Hz\n", frequency);
}