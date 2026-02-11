#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <libc/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========== Warna (RGB 24-bit, disimpan dalam uint32_t) ========== */
#define RGB(r, g, b)    (((r) << 16) | ((g) << 8) | (b))
#define RGBA(r,g,b,a)   (((r) << 16) | ((g) << 8) | (b) | ((a) << 24))
#define GET_R(color)    (((color) >> 16) & 0xFF)
#define GET_G(color)    (((color) >> 8) & 0xFF)
#define GET_B(color)    ((color) & 0xFF)
#define GET_A(color)    (((color) >> 24) & 0xFF)

#define COLOR_BLACK     0x00000000
#define COLOR_WHITE     0x00FFFFFF
#define COLOR_RED       0x00FF0000
#define COLOR_GREEN     0x0000FF00
#define COLOR_BLUE      0x000000FF
#define COLOR_YELLOW    0x00FFFF00
#define COLOR_CYAN      0x0000FFFF
#define COLOR_MAGENTA   0x00FF00FF

/* ========== Inisialisasi Framebuffer ========== */
void gfx_init(void* framebuffer, uint32_t width, uint32_t height, uint32_t pitch);

/* ========== Informasi Framebuffer ========== */
uint32_t gfx_width(void);
uint32_t gfx_height(void);
uint32_t gfx_pitch(void);
void*    gfx_buffer(void);

/* ========== Primitif Dasar ========== */
void gfx_putpixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t gfx_getpixel(uint32_t x, uint32_t y);

/* ========== Primitif Geometri ========== */
void gfx_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gfx_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void gfx_draw_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color);
void gfx_fill_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color);

/* ========== Blitting & Operasi Massal ========== */
void gfx_blit(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data);
void gfx_blit_alpha(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data, uint8_t alpha);
void gfx_clear(uint32_t color);
void gfx_clear_area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif
