// wine.cpp
#include "wine_structures.h"

// Low-level memory utilities for bare-metal deployment
void* memcpy(void* dest, const void* src, uint64_t count) {
    char* dst_char = (char*)dest;
    const char* src_char = (const char*)src;
    for (uint64_t i = 0; i < count; i++) {
        dst_char[i] = src_char[i];
    }
    return dest;
}

void* memset(void* dest, int val, uint64_t count) {
    char* dst_char = (char*)dest;
    for (uint64_t i = 0; i < count; i++) {
        dst_char[i] = (char)val;
    }
    return dest;
}

class WineEngine {
public:
    static bool execute_windows_binary(uint8_t* raw_pe_bytes) {
        // 1. Enforce validation of the legacy DOS stub
        auto* dos = (DOSHeader*)raw_pe_bytes;
        if (dos->magic != 0x5A4D) {
            return false; 
        }

        // 2. Validate the primary Portable Executable signature
        auto* pe = (PEHeader*)(raw_pe_bytes + dos->pe_header_offset);
        if (pe->signature != 0x00004550) {
            return false;
        }

        // 3. Extract the Optional Header info (assuming standard 64-bit PE32+)
        auto* optional = (PEOptionalHeader64*)((uint8_t*)pe + sizeof(PEHeader));
        if (optional->magic != 0x20B) {
            return false; // Fail if this binary is not 64-bit native
        }

        // 4. Extract where the image wants to be loaded in memory
        uint8_t* image_base = (uint8_t*)optional->image_base;
        
        // Zero-initialize the virtual memory location allocated for the image
        memset(image_base, 0, optional->size_of_image);

        // 5. Map headers into the start of the image base memory space
        memcpy(image_base, raw_pe_bytes, optional->size_of_headers);

        // 6. Iterate and copy headers to their specific relative virtual addresses (RVA)
        auto* section_headers = (PESectionHeader*)((uint8_t*)optional + pe->size_of_optional_header);
        
        for (int i = 0; i < pe->number_of_sections; i++) {
            PESectionHeader* section = &section_headers[i];
            
            // If the section has no data size in the file, it represents uninitialized memory (like .bss)
            if (section->size_of_raw_data == 0) {
                continue;
            }

            // Destination target memory location: Base Address + Section Relative Virtual Offset
            uint8_t* dest_target = image_base + section->virtual_address;
            
            // Source payload location: Raw File Base Address + Internal File Pointer Location
            uint8_t* src_payload = raw_pe_bytes + section->pointer_to_raw_data;

            // Load the section bytes directly into physical execution space
            memcpy(dest_target, src_payload, section->size_of_raw_data);
        }

        // 7. Resolve the absolute entry point memory pointer
        uint64_t absolute_entry_point = optional->image_base + optional->address_of_entry_point;

        // Define a function signature matching standard Win32 execution targets
        typedef uint32_t (__stdcall *WinMainFunc)();
        WinMainFunc entry_call = (WinMainFunc)absolute_entry_point;

        // 8. Hand execution over to the newly mapped Windows app
        entry_call();

        return true;
    }
};
