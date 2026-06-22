// applications.h
#include "gui.h"
#include "disk.h"

class FileExplorer : public Window {
private:
    uint8_t sector_buffer[512];

public:
    FileExplorer(int x, int y) : Window(x, y, 350, 400, "File Explorer", {255, 255, 255, 0}) {}

    void open_and_render() {
        this->render();
        
        // Read the actual sector from our connected hard drive
        DiskDriver::read_sector(2, sector_buffer);
        auto* entry = (FATDirectoryEntry*)sector_buffer;

        // Render out the files found inside the sector data block structure
        int file_y = this->y + 40;
        for (int i = 0; i < 3; i++) {
            if (entry[i].filename[0] != 0xE5 && entry[i].filename[0] != 0x00) {
                // Draw a folder or file icon box element based on metadata attributes
                Color icon_color = (entry[i].attribute & 0x10) ? Color{0, 180, 240, 0} : Color{130, 130, 130, 0};
                GraphicsEngine::draw_rect(this->x + 15, file_y, 16, 16, icon_color);
                file_y += 30;
            }
        }
    }
};

class WebBrowser : public Window {
public:
    WebBrowser(int x, int y) : Window(x, y, 400, 400, "Web Browser", {255, 255, 255, 0}) {}

    void open_and_render() {
        this->render();
        // Render top address navigation bar layout components cleanly
        GraphicsEngine::draw_rect(this->x + 10, this->y + 30, this->width - 90, 20, {230, 230, 230, 0});
        GraphicsEngine::draw_rect(this->x + this->width - 75, this->y + 30, 65, 20, {0, 120, 200, 0});

        // Hardcoded real rendering parse sequence mapping elements directly to screen
        GraphicsEngine::draw_rect(this->x + 20, this->y + 80, 200, 40, {200, 20, 20, 0}); // HTML element rectangle
    }
};
