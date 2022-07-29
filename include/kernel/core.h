#pragma once
#include <stdint.h>
#include <stddef.h>
//// core/idt.c core/isr.S
typedef struct {
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t	    ist;          // The IST in the TSS that the CPU will load into RSP; set to zero for now
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_mid;      // The higher 16 bits of the lower 32 bits of the ISR's address
	uint32_t    isr_high;     // The higher 32 bits of the ISR's address
	uint32_t    reserved;     // Set to zero
} __attribute__((packed)) idt_entry;
typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr;

void init_idt();
void load_idt(idtr* idtr_val);
void set_idt_entry(uint8_t vector, void* isr, uint8_t flags);
void hang_forever();
void hang_idle();

//// core/mm.c core/tlb.S
size_t alloc_page();
void dealloc_page(size_t page);
void init_mm();

size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags);
void unmap_page(size_t pmap, size_t vaddr);

void* kmalloc(size_t bytes);
void kdemalloc(void* data);

void tlb_invalidate(size_t page);
//// core/task.c core/context.S
typedef struct {
    size_t rsp, cr3;
} context;

typedef struct context_list{
    context c;
    struct context_list* next;
} context_list;

extern void context_switch();
extern void context_switch_nosave(size_t rsp, size_t cr3);
context switch_task(size_t rsp, size_t cr3);
void init_task();
void add_task(size_t rsp, size_t cr3);
void exit_task();
//// core/pic.c
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10	/* Special fully nested (not) */

void pic_set_mask(unsigned char IRQline);
void pic_clear_mask(unsigned char IRQline);
void pic_eoi(unsigned char irq);
void init_pic(int offset1, int offset2);

void init_pit(size_t frequency);