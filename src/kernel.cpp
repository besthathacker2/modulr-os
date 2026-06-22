// kernel.cpp
#include "applications.h"
#include "wine_structures.h"

extern "C" void init_64bit_paging();
void parse_multiboot_tags(uint64_t magic, uint64_t addr);

uint32_t* lfb = nullptr;
uint32_t screen_width = 0;
uint32_t screen_height = 0;
uint32_t screen_pitch = 0;

// Raw bytecode mimicking a Windows EXE header profile block pattern safely
uint8_t mock_windows_executable[64] = {
    0x4D, 0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // MZ Signature
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Pointer to PE location (0x28)
    0x50, 0x45, 0x00, 0x00, 0x64, 0x86, 0x02, 0x00  // PE Signature
};

extern "C" void kernelMain(uint64_t magic, uint64_t addr) {
    parse_multiboot_tags(magic, addr);
    if (lfb == nullptr) while(1);

    // Render OS Wallpaper background
    Color dark_teal = {60, 40, 20, 0};
    for(uint32_t y = 0; y < screen_height; ++y) {
        for(uint32_t x = 0; x < screen_width; ++x) {
            lfb[y * screen_pitch + x] = dark_teal.to_int();
        }
    }

    // 1. Fire up the native complete app classes
    FileExplorer explorer(40, 80);
    explorer.open_and_render();

    WebBrowser browser(420, 80);
    browser.open_and_render();

    // 2. Launch the Wine translation subsystem tracking interface
    WineEngine::execute_windows_binary(mock_windows_executable);

    while (1) {
        asm volatile("hlt");
    }
}
