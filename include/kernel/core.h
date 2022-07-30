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

//! @details - Sets default IDT entries,
//! - initializes the PIC,
//! - loads the IDT,
//! - and enables interrupts.
void init_idt();

//! @brief Loads IDT and enables interrupts.
//! @param idtr_val IDTR location.
void load_idt(idtr* idtr_val);

//! @brief Sets the entry in the IDT.
//! @param vector A vector number.
//! @param isr A pointer to code.
//! @param flags Flags.
void set_idt_entry(uint8_t vector, void* isr, uint8_t flags);

//! @brief Lock up completely.
void hang_forever();

//! @brief Go idle until an interrupt is recieved.
void hang_idle();

//// core/mm.c core/tlb.S

//! @brief Unlink page from freelist.
//! @return A page-aligned address in physical memory.
size_t alloc_page();

//! @brief Links page to freelist.
//! @param page A page-aligned address in physical memory.
void dealloc_page(size_t page);

//! @brief If a map of physical memory is unavailable, halts.
//! All usable memory should be added to the freelist.
void init_mm();

//! @brief Sets/updates a memory mapping, allocating memory if necessary.
//! @param pmap A page map.
//! @param vaddr A virtual address.
//! @param paddr A physical address.
//! @param flags Flags.
//! @return The same virtual address.
size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags);

//! @brief Removes a memory mapping if it exists.
//! @param pmap A page map.
//! @param vaddr A virtual address.
void unmap_page(size_t pmap, size_t vaddr);

void* kmalloc(size_t bytes);
void kdemalloc(void* data);

//! @brief Invalidates a page in the TLB.
//! @param page An address in virtual memory.
void tlb_invalidate(size_t page);

uint64_t get_pmap();

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

//! @brief Stores the arguments in the current task's structure,
//! then traverses to the next element of the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
//! @return A stack pointer and page map.
context switch_task(size_t rsp, size_t cr3);

//! @details - Sets up a circular list of tasks,
//! - context switches to initialize the first item,
//! - and enables a timer for further pre-emptive switches.
void init_task();

//! @brief Adds a task to the list.
//! @param rsp A stack pointer
//! @param cr3 A page map.
void add_task(size_t rsp, size_t cr3);

//! @brief Removes the current task from the list,
//! and switches to the next task up.
//! @return Does not return.
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

//! @brief Disallow IRQs from specified line. 
//! @param IRQline An IRQ line number.
void pic_set_mask(unsigned char IRQline);

//! @brief Allow IRQs from specified line. 
//! @param IRQline An IRQ line number.
void pic_clear_mask(unsigned char IRQline);

//! @brief Send end of interrupt signal to PIC.
//! @param irq An IRQ line number.
void pic_eoi(unsigned char irq);

//! @brief Set up PIC to use specified vectors.
//! @param offset1 Master PIC base vector.
//! @param offset2 Slave PIC base vector.
void init_pic(int offset1, int offset2);

//! @brief Sets the divider and mode of the PIT.
//! @param frequency A frequency in hertz.
void init_pit(size_t frequency);