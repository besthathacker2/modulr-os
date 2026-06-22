// applications.h
#pragma once
#include "gui.h"
#include "disk.h"

// Expose our real Wine binary execution method
class WineEngine {
public:
    static bool execute_windows_binary(uint8_t* raw_pe_bytes);
};

class FileExplorer : public Window {
private:
    uint8_t sector_buffer[512];
    // Allocation space in memory to buffer the raw file data from the drive
    uint8_t* file_load_buffer = (uint8_t*)0x2000000; 

public:
    FileExplorer(int x, int y) : Window(x, y, 350, 400, "File Explorer", {255, 255, 255, 0}) {}

    void open_and_render() {
        this->render();
        
        // Read Sector 2 (our root file system directory layer)
        DiskDriver::read_sector(2, sector_buffer);
        auto* entries = (FATDirectoryEntry*)sector_buffer;

        int file_y = this->y + 40;
        
        // Scan up to 10 filesystem entries inside the directory block
        for (int i = 0; i < 10; i++) {
            if (entries[i].filename[0] == 0xE5 || entries[i].filename[0] == 0x00) {
                continue; 
            }

            // Draw file item row icon indicator
            Color icon_color = (entries[i].attribute & 0x10) ? Color{0, 180, 240, 0} : Color{130, 130, 130, 0};
            GraphicsEngine::draw_rect(this->x + 15, file_y, 16, 16, icon_color);
            file_y += 30;

            // Check if this file is a Windows Executable (.EXE)
            if (entries[i].filename[8] == 'E' && entries[i].filename[9] == 'X' && entries[i].filename[10] == 'E') {
                
                // Calculate total sectors required to load the entire executable file
                uint32_t sectors_to_read = (entries[i].file_size + 512 - 1) / 512;
                
                // Read the file data into our staging buffer
                DiskDriver::read_file_sectors(entries[i].first_cluster_low, sectors_to_read, file_load_buffer);
                
                // Route the loaded binary payload straight into our Wine execution loader!
                WineEngine::execute_windows_binary(file_load_buffer);
            }
        }
    }
};

class WebBrowser : public Window {
public:
    WebBrowser(int x, int y) : Window(x, y, 400, 400, "Web Browser", {255, 255, 255, 0}) {}
    void open_and_render() {
        this->render();
        GraphicsEngine::draw_rect(this->x + 10, this->y + 30, this->width - 90, 20, {230, 230, 230, 0});
        GraphicsEngine::draw_rect(this->x + this->width - 75, this->y + 30, 65, 20, {0, 120, 200, 0});
        GraphicsEngine::draw_rect(this->x + 20, this->y + 80, 200, 40, {200, 20, 20, 0}); 
    }
};
