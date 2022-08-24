#pragma once
#include <stddef.h>
#include <stdint.h>
extern uint64_t hhdm;
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

void* heap_allocate(size_t bytes);

void heap_deallocate(void* ptr);

void* page_allocate(void* address);

void page_deallocate(void* address);

//! @brief Invalidates a page in the TLB.
//! @param page An address in virtual memory.
void tlb_invalidate(size_t page);

extern size_t get_pmap();

size_t new_pmap();