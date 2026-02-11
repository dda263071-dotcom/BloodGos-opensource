#ifndef LIB_UI_GRAPHICS_H
#define LIB_UI_GRAPHICS_H

#include <libc/types.h>
#include <libc/math/math.h>   /* untuk fixed_t, fixed_sin, fixed_cos, dll */

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
   Warna & Format Pixel (32-bit ARGB / RGB)
   ============================================================================ */
#define RGB(r, g, b)        (((r) << 16) | ((g) << 8) | (b))
#define RGBA(r, g, b, a)    (((r) << 16) | ((g) << 8) | (b) | ((a) << 24))
#define GET_R(color)        (((color) >> 16) & 0xFF)
#define GET_G(color)        (((color) >> 8) & 0xFF)
#define GET_B(color)        ((color) & 0xFF)
#define GET_A(color)        (((color) >> 24) & 0xFF)

/* Warna dasar (tanpa alpha) */
#define COLOR_BLACK         0x00000000
#define COLOR_WHITE         0x00FFFFFF
#define COLOR_RED           0x00FF0000
#define COLOR_GREEN         0x0000FF00
#define COLOR_BLUE          0x000000FF
#define COLOR_YELLOW        0x00FFFF00
#define COLOR_CYAN          0x0000FFFF
#define COLOR_MAGENTA       0x00FF00FF
#define COLOR_GRAY          0x00808080
#define COLOR_LIGHT_GRAY    0x00C0C0C0
#define COLOR_DARK_GRAY     0x00404040

/* ============================================================================
   State & Inisialisasi Framebuffer
   ============================================================================ */
void        gfx_init(void* framebuffer, uint32_t width, uint32_t height, uint32_t pitch);
uint32_t    gfx_width(void);
uint32_t    gfx_height(void);
uint32_t    gfx_pitch(void);      /* dalam pixel (bukan byte) */
void*       gfx_buffer(void);

/* ============================================================================
   Primitif Dasar
   ============================================================================ */
void        gfx_putpixel(uint32_t x, uint32_t y, uint32_t color);
uint32_t    gfx_getpixel(uint32_t x, uint32_t y);

/* ============================================================================
   Primitif Geometri Dasar
   ============================================================================ */
void        gfx_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void        gfx_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void        gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color);
void        gfx_draw_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color);
void        gfx_fill_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color);

/* ============================================================================
   Blitting & Operasi Massal
   ============================================================================ */
void        gfx_blit(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data);
void        gfx_blit_alpha(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data, uint8_t alpha);
void        gfx_clear(uint32_t color);
void        gfx_clear_area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/* ============================================================================
   Rounded Rectangle
   ============================================================================ */
void        gfx_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t color);
void        gfx_fill_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t color);

/* ============================================================================
   Polygon (titik disimpan berpasangan [x0,y0,x1,y1,...])
   ============================================================================ */
void        gfx_draw_polygon(const int32_t* points, uint32_t count, uint32_t color);
void        gfx_fill_polygon(const int32_t* points, uint32_t count, uint32_t color);
void        gfx_fill_convex_polygon(const int32_t* points, uint32_t count, uint32_t color);

/* ============================================================================
   Bezier Curve (cubic)
   ============================================================================ */
void        gfx_draw_bezier(int32_t x0, int32_t y0,
                            int32_t x1, int32_t y1,
                            int32_t x2, int32_t y2,
                            int32_t x3, int32_t y3,
                            uint32_t color);

/* ============================================================================
   Arc (bagian dari lingkaran, sudut dalam derajat)
   ============================================================================ */
void        gfx_draw_arc(int32_t xc, int32_t yc, int32_t r,
                         int32_t start_deg, int32_t end_deg,
                         uint32_t color);
void        gfx_fill_arc(int32_t xc, int32_t yc, int32_t r,
                         int32_t start_deg, int32_t end_deg,
                         uint32_t color);

/* ============================================================================
   Ellipse
   ============================================================================ */
void        gfx_draw_ellipse(int32_t xc, int32_t yc, int32_t a, int32_t b, uint32_t color);
void        gfx_fill_ellipse(int32_t xc, int32_t yc, int32_t a, int32_t b, uint32_t color);

#ifdef __cplusplus
}
#endif

#endif /* LIB_UI_GRAPHICS_H */
