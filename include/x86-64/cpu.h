#pragma once
#include <stdint.h>

typedef struct {
	uint32_t unused0;
	uint64_t rsp[3];
	uint64_t unused1;
	uint64_t ist[7];
	uint64_t unused2;
	uint16_t unused3;
	uint16_t iopb;
} __attribute__((packed)) tss;

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
} __attribute__((packed)) gdtr;

typedef gdtr idtr;

//! @brief Builds GDT entry.
//! @param entry Where to place the entry. A TSS takes the size of two entries.
//! @param base Address of TSS if entry is for TSS.
//! @param limit Size of TSS if entry is for TSS.
//! @param access Access specifier for entry.
//! @param flags Flags for entry.
void set_gdt_entry(uint64_t* entry, uint64_t base, uint16_t limit, uint8_t access, uint8_t flags);

//! @brief Builds IDT entry.
//! @param vector Interrupt vector in table.
//! @param isr Address of ISR.
//! @param flags Flags for entry.
void set_idt_entry(uint8_t vector, void* isr, uint8_t flags);

//! @brief Builds IDT entry and clears mask for IRQ.
//! @param irq IRQ line.
//! @param isr Address of ISR.
//! @param flags Flags for entry.
void install_irq_handler(uint8_t irq, void* isr, uint8_t flags);

//! @brief Loads GDTR register.
//! @param gdtr_gdt Address of GDTR structure.
void load_gdt(gdtr* gdtr_gdt);
void load_idt(idtr* idtr_gdt);
void load_tss();

extern void die();
extern void idle();

void init_gdt();
void init_idt();