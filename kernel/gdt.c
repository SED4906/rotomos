#include <kernel/gdt.h>
#include <kernel/libc.h>
#include <stdint.h>
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
}