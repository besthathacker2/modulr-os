// gdt.cpp
#include <stdint.h>

struct GDTEntry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

struct GDTPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

GDTEntry gdt[5];
GDTPtr gdt_ptr;

// Assembly helper defined in assembly to flush the new GDT registers
extern "C" void gdt_flush(uint32_t);

void set_gdt_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt[num].base_low    = (base & 0...FFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

extern "C" void init_gdt() {
    gdt_ptr.limit = (sizeof(GDTEntry) * 5) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    set_gdt_gate(0, 0, 0, 0, 0);                // Null segment
    set_gdt_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code segment
    set_gdt_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data segment
    set_gdt_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User Code segment
    set_gdt_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User Data segment

    gdt_flush((uint32_t)&gdt_ptr);
}
