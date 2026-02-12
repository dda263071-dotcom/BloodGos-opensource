#include <lib_ui/font/font.h>
#include <lib_ui/graphics/graphics.h>
#include <libc/string.h>

/* Draw single glyph */
static void draw_glyph(font_t* font, uint32_t x, uint32_t y, uint8_t glyph_index, uint32_t color) {
    if (!font || !font->glyph_data) return;
    
    uint32_t bpg = font->bytes_per_glyph;
    uint32_t w = font->width;
    uint32_t h = font->height;
    const uint8_t* glyph = font->glyph_data + (glyph_index * bpg);
    
    for (uint32_t row = 0; row < h; row++) {
        uint32_t mask = 0x80;  /* bit MSB dulu */
        uint32_t val;
        
        if (font->is_psf2) {
            /* PSF2: bits per pixel? kita asumsikan 1-bit */
            /* Untuk PSF2 1-bit, byte per row = (w+7)/8 */
            uint32_t bytes_per_row = (w + 7) / 8;
            val = glyph[row * bytes_per_row];
        } else {
            /* PSF1 / builtin: 1 byte per row, 8 pixel */
            val = glyph[row];
        }
        
        for (uint32_t col = 0; col < w; col++) {
            if (val & mask) {
                gfx_putpixel(x + col, y + row, color);
            }
            mask >>= 1;
            if (mask == 0) {
                mask = 0x80;
                if (font->is_psf2) {
                    /* ambil byte berikutnya jika lebar > 8 */
                    uint32_t bytes_per_row = (w + 7) / 8;
                    uint32_t byte_index = (col+1) / 8;
                    if (byte_index < bytes_per_row) {
                        val = glyph[row * bytes_per_row + byte_index];
                    }
                }
            }
        }
    }
}

void font_draw_char(font_t* font, uint32_t x, uint32_t y, char c, uint32_t color) {
    draw_glyph(font, x, y, (uint8_t)c, color);
}

void font_draw_string(font_t* font, uint32_t x, uint32_t y, const char* str, uint32_t color) {
    if (!font || !str) return;
    uint32_t cur_x = x;
    while (*str) {
        font_draw_char(font, cur_x, y, *str, color);
        cur_x += font->width;
        str++;
    }
}

void font_draw_string_bg(font_t* font, uint32_t x, uint32_t y, const char* str, uint32_t fg, uint32_t bg) {
    if (!font || !str) return;
    uint32_t cur_x = x;
    uint32_t w = font->width;
    uint32_t h = font->height;
    
    while (*str) {
        /* Draw background */
        gfx_fill_rect(cur_x, y, w, h, bg);
        /* Draw character */
        font_draw_char(font, cur_x, y, *str, fg);
        cur_x += w;
        str++;
    }
}

uint32_t font_string_width(font_t* font, const char* str) {
    if (!font || !str) return 0;
    return font->width * strlen(str);
}
