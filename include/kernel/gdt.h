#pragma once
#include <stdint.h>
extern "C" struct tss {
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
} __attribute__((packed));

extern "C" struct gdtr {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed));

void GdtInitialize();
void GdtSet(uint64_t* entry, uint64_t base, uint16_t limit, uint8_t access, uint8_t flags);
extern "C" void GdtLoad(gdtr* gdtr_gdt);
extern "C" void TssLoad(tss* tss_gdt);