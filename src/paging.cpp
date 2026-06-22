// paging.cpp
#include <stdint.h>

// Align tables to 4096-byte boundaries as required by hardware
uint64_t pml4[512] __attribute__((aligned(4096)));
uint64_t pdpt[512] __attribute__((aligned(4096)));
uint64_t page_directory[512] __attribute__((aligned(4096)));

extern "C" void init_64bit_paging() {
    // Clear the tables to ensure no garbage memory addresses are evaluated
    for(int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pdpt[i] = 0;
        page_directory[i] = 0;
    }

    // Map entry 0 of PML4 to point directly to our PDPT table
    // Attributes: 0x03 = Present + Read/Write
    pml4[0] = ((uint64_t)pdpt) | 0x03;

    // Map entry 0 of PDPT to point directly to our Page Directory
    pdpt[0] = ((uint64_t)page_directory) | 0x03;

    // Use large 2 MB identity pages to cleanly map the first 1 GB block of physical memory
    // Attribute 0x83 = Present + Read/Write + Page Size (Huge Page Bit)
    for(int i = 0; i < 512; i++) {
        page_directory[i] = ((uint64_t)i * 0x200000) | 0x83;
    }
}
