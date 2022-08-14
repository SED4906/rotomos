#pragma once
#include <stdint.h>

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

