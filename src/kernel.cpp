// kernel.cpp
#include "applications.h"

extern "C" void init_64bit_paging();
void parse_multiboot_tags(uint64_t magic, uint64_t addr);

uint32_t* lfb = nullptr;
uint32_t screen_width = 0;
uint32_t screen_height = 0;
uint32_t screen_pitch = 0;

extern "C" void kernelMain(uint64_t magic, uint64_t addr) {
    parse_multiboot_tags(magic, addr);
    if (lfb == nullptr) while(1);

    // Render Solid Operating System Teal Desktop Canvas Wallpaper
    Color dark_teal = {60, 40, 20, 0};
    for(uint32_t y = 0; y < screen_height; ++y) {
        for(uint32_t x = 0; x < screen_width; ++x) {
            lfb[y * screen_pitch + x] = dark_teal.to_int();
        }
    }

    // Launch the active native File Explorer app module instance
    FileExplorer explorer(40, 80);
    explorer.open_and_render();

    // Launch parallel native application window interfaces
    WebBrowser browser(420, 80);
    browser.open_and_render();

    // System active loop - listening to hardware state allocations
    while (1) {
        asm volatile("hlt");
    }
}
