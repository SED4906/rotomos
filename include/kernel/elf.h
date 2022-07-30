#pragma once
#include <stdint.h>
typedef struct {
    uint32_t magic;
    uint8_t bits;
    uint8_t endian;
    uint8_t header_version;
    uint8_t abi;
    uint64_t unused;
    uint16_t form;
    uint16_t isa;
    uint32_t elf_version;
    uint64_t entry_point;
    uint64_t pht_location;
    uint64_t sht_location;
    uint32_t flags;
    uint16_t header_size;
    uint16_t pht_entry_size;
    uint16_t pht_entry_num;
    uint16_t sht_entry_size;
    uint16_t sht_entry_num;
    uint16_t sht_name_index;
} __attribute__((packed)) elf_header;

typedef struct {
    uint32_t segment_type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t unused;
    uint64_t size_file;
    uint64_t size_memory;
    uint64_t alignment;
} __attribute__((packed)) elf_pht_entry;
