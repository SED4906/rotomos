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

//! @brief - Sets default IDT entries,
//! - initializes the PIC,
//! - loads the IDT,
//! - and enables interrupts.
void init_idt();

//! @brief Loads IDT and enables interrupts.
//! @param idtr_val IDTR location.
void load_idt(idtr* idtr_val);

//! @brief Loads IDT and enables interrupts again.
void reload_idt();

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
//! @return The page table entry.
size_t unmap_page(size_t pmap, size_t vaddr);

//! @brief Allocate some bytes from a heap.
//! @param bytes How many bytes to allocate.
//! @return An address in virtual memory.
void* kmalloc(size_t bytes);

//! @brief Deallocate from a heap.
//! @param data The address of the allocation.
void kdemalloc(void* data);

//! @brief Invalidates a page in the TLB.
//! @param page An address in virtual memory.
void tlb_invalidate(size_t page);

size_t get_pmap();

size_t new_pmap();

size_t get_hhdm();

//// core/task.c core/context.S
typedef struct {
    size_t rsp, cr3;
} context;

typedef struct context_list{
    context c;
	int pid;
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

//! @brief - Sets up a circular list of tasks,
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
//// core/gdt.c core/tables.S
typedef struct {
	uint32_t u0;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t u1;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t u2;
	uint16_t u3;
	uint16_t iopb;
} tss;

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) gdtr;

void set_gdt_entry(uint64_t* entry, uint64_t base, uint16_t limit, uint8_t access, uint8_t flags);

void init_gdt();

void load_gdt(gdtr* gdtr_gdt);

void load_tss();
//// core/fb.c misc_fun.c
void fb_clear_screen();

void fb_plot(uint16_t x, uint16_t y, uint32_t rgb);

void fb_plot_line(uint64_t x0, uint64_t y0, uint64_t x1, uint64_t y1, uint32_t rgba);

void fb_draw_rotom_logo(uint64_t offset_x,uint64_t offset_y);

void fb_draw_rotom_text(uint64_t offset_x,uint64_t offset_y);

size_t fb_print_string(const char * str, size_t len);
//// core/pci.c

void init_pci();

void pci_check_function(uint8_t bus, uint8_t device, uint8_t function);

void pci_check_device(uint8_t bus, uint8_t device);

void pci_check_bus(uint8_t bus);