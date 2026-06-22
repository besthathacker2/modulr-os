// disk.h
#pragma once
#include <stdint.h>

class DiskDriver {
private:
    static inline void outb(uint16_t port, uint8_t val) {
        asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
    }
    static inline uint8_t inb(uint16_t port) {
        uint8_t ret;
        asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }
    static inline uint16_t inw(uint16_t port) {
        uint16_t ret;
        asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
        return ret;
    }

public:
    static void read_sector(uint32_t lba, uint8_t* target_buffer) {
        outb(0x1F6, (0xE0 | ((lba >> 24) & 0x0F)));
        outb(0x1F2, 1); 
        outb(0x1F3, (uint8_t)lba);
        outb(0x1F4, (uint8_t)(lba >> 8));
        outb(0x1F5, (uint8_t)(lba >> 16));
        outb(0x1F7, 0x20); 

        while ((inb(0x1F7) & 0x88) != 0x08);

        uint16_t* ptr = (uint16_t*)target_buffer;
        for (int i = 0; i < 256; i++) {
            ptr[i] = inw(0x1F0);
        }
    }

    // Reads multiple sequential sectors into a continuous destination memory block
    static void read_file_sectors(uint32_t start_lba, uint32_t sector_count, uint8_t* dest_buffer) {
        for (uint32_t i = 0; i < sector_count; i++) {
            read_sector(start_lba + i, dest_buffer + (i * 512));
        }
    }
};

struct FATDirectoryEntry {
    uint8_t  filename[11]; // 8 chars name, 3 chars extension
    uint8_t  attribute;
    uint8_t  reserved[10];
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t first_cluster_low; // Maps to LBA start sector
    uint32_t file_size;
} __attribute__((packed));
