#include <lib_ui/graphics/graphics.h>
#include <libc/string.h>   /* untuk memcpy, memset */
#include <libc/math/math.h>/* opsional, untuk circle fixed-point */

/* ========== State Framebuffer Global ========== */
static uint32_t* fb = NULL;
static uint32_t fb_width = 0;
static uint32_t fb_height = 0;
static uint32_t fb_pitch = 0;  /* dalam pixel (bukan byte) */

/* ========== Inisialisasi ========== */
void gfx_init(void* framebuffer, uint32_t width, uint32_t height, uint32_t pitch) {
    fb = (uint32_t*)framebuffer;
    fb_width = width;
    fb_height = height;
    fb_pitch = pitch / 4;  /* konversi byte ke pixel (asumsi 32bpp) */
}

uint32_t gfx_width(void)   { return fb_width; }
uint32_t gfx_height(void)  { return fb_height; }
uint32_t gfx_pitch(void)   { return fb_pitch; }
void*    gfx_buffer(void)  { return fb; }

/* ========== Primitif Dasar ========== */
void gfx_putpixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x >= fb_width || y >= fb_height) return;
    fb[y * fb_pitch + x] = color;
}

uint32_t gfx_getpixel(uint32_t x, uint32_t y) {
    if (x >= fb_width || y >= fb_height) return 0;
    return fb[y * fb_pitch + x];
}

/* ========== Persegi Panjang ========== */
void gfx_draw_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (w == 0 || h == 0) return;
    /* Batasi dalam layar */
    if (x >= fb_width || y >= fb_height) return;
    if (x + w > fb_width)  w = fb_width - x;
    if (y + h > fb_height) h = fb_height - y;

    uint32_t i;
    /* Garis atas */
    for (i = 0; i < w; i++) fb[y * fb_pitch + (x + i)] = color;
    /* Garis bawah */
    if (h > 1) {
        uint32_t last_y = y + h - 1;
        for (i = 0; i < w; i++) fb[last_y * fb_pitch + (x + i)] = color;
    }
    /* Garis kiri & kanan */
    for (i = 1; i < h - 1; i++) {
        fb[(y + i) * fb_pitch + x] = color;
        if (w > 1) fb[(y + i) * fb_pitch + (x + w - 1)] = color;
    }
}

void gfx_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    if (w == 0 || h == 0) return;
    if (x >= fb_width || y >= fb_height) return;
    if (x + w > fb_width)  w = fb_width - x;
    if (y + h > fb_height) h = fb_height - y;

    for (uint32_t row = 0; row < h; row++) {
        uint32_t* line = &fb[(y + row) * fb_pitch + x];
        for (uint32_t col = 0; col < w; col++) {
            line[col] = color;
        }
    }
}

/* ========== Garis (Bresenham) ========== */
void gfx_draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t color) {
    int32_t dx = x2 - x1, dy = y2 - y1;
    int32_t step_x = dx >= 0 ? 1 : -1, step_y = dy >= 0 ? 1 : -1;
    dx = dx >= 0 ? dx : -dx;
    dy = dy >= 0 ? dy : -dy;

    int32_t x = x1, y = y1;
    gfx_putpixel(x, y, color);

    if (dx >= dy) {
        int32_t err = 2 * dy - dx;
        while (x != x2) {
            x += step_x;
            if (err >= 0) {
                y += step_y;
                err -= 2 * dx;
            }
            err += 2 * dy;
            gfx_putpixel(x, y, color);
        }
    } else {
        int32_t err = 2 * dx - dy;
        while (y != y2) {
            y += step_y;
            if (err >= 0) {
                x += step_x;
                err -= 2 * dy;
            }
            err += 2 * dx;
            gfx_putpixel(x, y, color);
        }
    }
}

/* ========== Lingkaran (Midpoint) ========== */
void gfx_draw_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color) {
    int32_t x = 0, y = r;
    int32_t d = 1 - r;
    while (x <= y) {
        gfx_putpixel(xc + x, yc + y, color);
        gfx_putpixel(xc + y, yc + x, color);
        gfx_putpixel(xc - x, yc + y, color);
        gfx_putpixel(xc - y, yc + x, color);
        gfx_putpixel(xc + x, yc - y, color);
        gfx_putpixel(xc + y, yc - x, color);
        gfx_putpixel(xc - x, yc - y, color);
        gfx_putpixel(xc - y, yc - x, color);

        if (d < 0) {
            d += 2 * x + 3;
        } else {
            d += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

void gfx_fill_circle(int32_t xc, int32_t yc, int32_t r, uint32_t color) {
    for (int32_t y = -r; y <= r; y++) {
        for (int32_t x = -r; x <= r; x++) {
            if (x*x + y*y <= r*r) {
                gfx_putpixel(xc + x, yc + y, color);
            }
        }
    }
}

/* ========== Blitting ========== */
void gfx_blit(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data) {
    if (w == 0 || h == 0) return;
    if (x >= fb_width || y >= fb_height) return;
    if (x + w > fb_width)  w = fb_width - x;
    if (y + h > fb_height) h = fb_height - y;

    for (uint32_t row = 0; row < h; row++) {
        uint32_t* dest = &fb[(y + row) * fb_pitch + x];
        const uint32_t* src = &data[row * w];
        memcpy(dest, src, w * sizeof(uint32_t));
    }
}

void gfx_blit_alpha(uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint32_t* data, uint8_t alpha) {
    if (w == 0 || h == 0) return;
    if (x >= fb_width || y >= fb_height) return;
    if (x + w > fb_width)  w = fb_width - x;
    if (y + h > fb_height) h = fb_height - y;

    for (uint32_t row = 0; row < h; row++) {
        for (uint32_t col = 0; col < w; col++) {
            uint32_t src = data[row * w + col];
            uint32_t dst = fb[(y + row) * fb_pitch + (x + col)];
            
            uint8_t a = (alpha * GET_A(src)) >> 8;
            uint8_t inv_a = 255 - a;

            uint8_t r = (GET_R(src) * a + GET_R(dst) * inv_a) >> 8;
            uint8_t g = (GET_G(src) * a + GET_G(dst) * inv_a) >> 8;
            uint8_t b = (GET_B(src) * a + GET_B(dst) * inv_a) >> 8;

            fb[(y + row) * fb_pitch + (x + col)] = RGB(r, g, b);
        }
    }
}

/* ========== Operasi Layar ========== */
void gfx_clear(uint32_t color) {
    for (uint32_t y = 0; y < fb_height; y++) {
        uint32_t* line = &fb[y * fb_pitch];
        for (uint32_t x = 0; x < fb_width; x++) {
            line[x] = color;
        }
    }
}

void gfx_clear_area(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
    gfx_fill_rect(x, y, w, h, color);
}
