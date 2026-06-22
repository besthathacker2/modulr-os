// gui.h
#include <stdint.h>

extern uint32_t* lfb;
extern uint32_t screen_width;
extern uint32_t screen_pitch;

struct Color {
    uint8_t b, g, r, a;
    uint32_t to_int() const { return (r << 16) | (g << 8) | b; }
};

class GraphicsEngine {
public:
    static void put_pixel(int x, int y, Color color) {
        if (x >= 0 && x < (int)screen_width && y >= 0) {
            lfb[y * screen_pitch + x] = color.to_int();
        }
    }

    static void draw_rect(int x, int y, int width, int height, Color color) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                put_pixel(x + j, y + i, color);
            }
        }
    }

    // A hardcoded 8x8 bitmap font array to remove string representation placeholders safely
    static void draw_char(int x, int y, char c, Color color) {
        static const uint8_t font[2][8] = {
            {0x3C, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x00}, // 'A'
            {0x3C, 0x42, 0x40, 0x40, 0x40, 0x42, 0x3C, 0x00}  // 'C'
        };
        int index = (c == 'C') ? 1 : 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (font[index][i] & (1 << (7 - j))) {
                    put_pixel(x + j, y + i, color);
                }
            }
        }
    }
};

class Window {
public:
    int x, y, width, height;
    const char* title;
    Color bg_color;

    Window(int nx, int ny, int nw, int nh, const char* ntitle, Color nbg)
        : x(nx), y(ny), width(nw), height(nh), title(ntitle), bg_color(nbg) {}

    void render() {
        GraphicsEngine::draw_rect(x, y, width, height, bg_color);
        GraphicsEngine::draw_rect(x, y, width, 24, {40, 70, 120, 0}); // Blue titlebar
        GraphicsEngine::draw_char(x + 6, y + 8, 'A', {255, 255, 255, 0}); // Text engine draw
    }
};
