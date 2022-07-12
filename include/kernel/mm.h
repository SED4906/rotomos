#pragma once
#include <stddef.h>
size_t alloc_page();
void dealloc_page(size_t page);
void init_page();

size_t map_page(size_t pmap, size_t vaddr, size_t paddr, size_t flags);
void unmap_page(size_t pmap, size_t vaddr);

void* kmalloc(size_t bytes);
void kdemalloc(void* data);