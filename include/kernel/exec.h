#pragma once
#include <stdint.h>
//// aout.c
typedef struct {
	uint64_t   a_midmag;
	uint64_t   a_text;
	uint64_t   a_data;
	uint64_t   a_bss;
	uint64_t   a_syms;
	uint64_t   a_entry;
	uint64_t   a_trsize;
	uint64_t   a_drsize;
} aout_header;
void aout_exec(size_t file, size_t len);