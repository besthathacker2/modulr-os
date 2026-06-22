// disk.h
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
    // Reads a 512-byte sector from the hard drive using ATA PIO mode
    static void read_sector(uint32_t lba, uint8_t* target_buffer) {
        outb(0x1F6, (0xE0 | ((lba >> 24) & 0x0F)));
        outb(0x1F2, 1); // Sector count (1 sector)
        outb(0x1F3, (uint8_t)lba);
        outb(0x1F4, (uint8_t)(lba >> 8));
        outb(0x1F5, (uint8_t)(lba >> 16));
        outb(0x1F7, 0x20); // Command 0x20 = Read Sectors with retry

        // Poll till the status drive drops BSY (Busy) and sets DRQ (Data Request)
        while ((inb(0x1F7) & 0x88) != 0x08);

        // Read 256 words (512 bytes) from the data port
        uint16_t* ptr = (uint16_t*)target_buffer;
        for (int i = 0; i < 256; i++) {
            ptr[i] = inw(0x1F0);
        }
    }
};

// Structures for parsing a true FAT storage layer
struct FATDirectoryEntry {
    uint8_t  filename[11];
    uint8_t  attribute;
    uint8_t  reserved;
    uint8_t  creation_time_ms;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed));
