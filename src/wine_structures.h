// wine_structures.h
#include <stdint.h>

struct DOSHeader {
    uint16_t magic;                  // Must be 'MZ' (0x5A4D)
    uint16_t last_page_bytes;
    uint16_t pages;
    uint16_t relocations;
    uint16_t header_paragraphs;
    uint16_t min_extra_paragraphs;
    uint16_t max_extra_paragraphs;
    uint16_t ss_reg;
    uint16_t sp_reg;
    uint16_t checksum;
    uint16_t ip_reg;
    uint16_t cs_reg;
    uint16_t reloc_table_offset;
    uint16_t overlay_number;
    uint16_t reserved[4];
    uint16_t oem_id;
    uint16_t oem_info;
    uint16_t reserved2[10];
    uint32_t pe_header_offset;       // Offset to PE Header
} __attribute__((packed));

struct PEHeader {
    uint32_t signature;              // Must be 'PE\0\0' (0x00004550)
    uint16_t machine;
    uint16_t number_of_sections;
    uint32_t time_date_stamp;
    uint32_t pointer_to_symbol_table;
    uint32_t number_of_symbols;
    uint16_t size_of_optional_header;
    uint16_t characteristics;
} __attribute__((packed));

struct DataDirectory {
    uint32_t virtual_address;
    uint32_t size;
} __attribute__((packed));

struct PEOptionalHeader64 {
    uint16_t magic;                  // 0x20B for PE32+ (64-bit)
    uint8_t  major_linker_version;
    uint8_t  minor_linker_version;
    uint32_t size_of_code;
    uint32_t size_of_initialized_data;
    uint32_t size_of_uninitialized_data;
    uint32_t address_of_entry_point; // RVA of the entry point
    uint32_t base_of_code;
    uint64_t image_base;             // Preferred load address
    uint32_t section_alignment;
    uint32_t file_alignment;
    uint16_t major_operating_system_version;
    uint16_t minor_operating_system_version;
    uint16_t major_image_version;
    uint16_t minor_image_version;
    uint16_t major_subsystem_version;
    uint16_t minor_subsystem_version;
    uint32_t win32_version_value;
    uint32_t size_of_image;          // Total memory required
    uint32_t size_of_headers;
    uint32_t check_sum;
    uint16_t subsystem;
    uint16_t dll_characteristics;
    uint64_t size_of_stack_reserve;
    uint64_t size_of_stack_commit;
    uint64_t size_of_heap_reserve;
    uint64_t size_of_heap_commit;
    uint32_t loader_flags;
    uint32_t number_of_rva_and_sizes;
    DataDirectory data_directories[16];
} __attribute__((packed));

struct PESectionHeader {
    uint8_t  name[8];
    uint32_t virtual_size;
    uint32_t virtual_address;       // Target Virtual Address (RVA)
    uint32_t size_of_raw_data;       // Size in the file
    uint32_t pointer_to_raw_data;    // Offset in the file
    uint32_t pointer_to_relocations;
    uint32_t pointer_to_linenumbers;
    uint16_t number_of_relocations;
    uint16_t number_of_linenumbers;
    uint32_t characteristics;
} __attribute__((packed));
