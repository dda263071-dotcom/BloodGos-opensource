#include <lib_ui/graphics/graphics.h>
#include <libc/math/math.h>   /* fixed_sin, fixed_cos, fixed_from_int, fixed_mul, fixed_div, fixed_to_int */
#include <libc/string.h>      /* memcpy, memset (optional) */

/* ========== Helper: swap dua integer ========== */
static void swap(int32_t* a, int32_t* b) {
    int32_t t = *a;
    *a = *b;
    *b = t;
}

/* ========== ROUNDED RECTANGLE ========== */
void gfx_draw_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t color) {
    if (r == 0) {
        gfx_draw_rect(x, y, w, h, color);
        return;
    }
    if (r > w/2) r = w/2;
    if (r > h/2) r = h/2;

    // Garis horizontal atas & bawah
    gfx_draw_line(x + r, y, x + w - r - 1, y, color);
    gfx_draw_line(x + r, y + h - 1, x + w - r - 1, y + h - 1, color);
    // Garis vertikal kiri & kanan
    gfx_draw_line(x, y + r, x, y + h - r - 1, color);
    gfx_draw_line(x + w - 1, y + r, x + w - 1, y + h - r - 1, color);
    // 4 sudut lingkaran
    gfx_draw_circle(x + r, y + r, r, color);
    gfx_draw_circle(x + w - r - 1, y + r, r, color);
    gfx_draw_circle(x + r, y + h - r - 1, r, color);
    gfx_draw_circle(x + w - r - 1, y + h - r - 1, r, color);
}

void gfx_fill_rounded_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t color) {
    if (r == 0) {
        gfx_fill_rect(x, y, w, h, color);
        return;
    }
    if (r > w/2) r = w/2;
    if (r > h/2) r = h/2;

    // Isi persegi tengah
    gfx_fill_rect(x, y + r, w, h - 2*r, color);
    gfx_fill_rect(x + r, y, w - 2*r, r, color);
    gfx_fill_rect(x + r, y + h - r, w - 2*r, r, color);
    // 4 sudut lingkaran penuh
    gfx_fill_circle(x + r, y + r, r, color);
    gfx_fill_circle(x + w - r - 1, y + r, r, color);
    gfx_fill_circle(x + r, y + h - r - 1, r, color);
    gfx_fill_circle(x + w - r - 1, y + h - r - 1, r, color);
}

/* ========== POLYGON ========== */
void gfx_draw_polygon(const int32_t* points, uint32_t count, uint32_t color) {
    if (count < 2) return;
    for (uint32_t i = 0; i < count - 1; i++) {
        gfx_draw_line(points[i*2], points[i*2+1], points[(i+1)*2], points[(i+1)*2+1], color);
    }
    // tutup polygon
    gfx_draw_line(points[(count-1)*2], points[(count-1)*2+1], points[0], points[1], color);
}

/* Fill convex polygon menggunakan triangle fan */
void gfx_fill_convex_polygon(const int32_t* points, uint32_t count, uint32_t color) {
    if (count < 3) return;
    // pilih titik pertama sebagai pivot
    int32_t x0 = points[0];
    int32_t y0 = points[1];
    for (uint32_t i = 1; i < count - 1; i++) {
        // gambar segitiga (x0,y0) - (x1,y1) - (x2,y2)
        int32_t x1 = points[i*2];
        int32_t y1 = points[i*2+1];
        int32_t x2 = points[(i+1)*2];
        int32_t y2 = points[(i+1)*2+1];
        
        // scanline fill untuk segitiga (sederhana, tidak dioptimasi)
        // cari ymin, ymax
        int32_t ymin = y0, ymax = y0;
        if (y1 < ymin) ymin = y1; if (y1 > ymax) ymax = y1;
        if (y2 < ymin) ymin = y2; if (y2 > ymax) ymax = y2;
        if (ymin < 0) ymin = 0;
        if (ymax >= gfx_height()) ymax = gfx_height() - 1;
        
        for (int32_t y = ymin; y <= ymax; y++) {
            // hitung perpotongan garis dengan scanline y
            int32_t xl = INT32_MAX, xr = INT32_MIN;
            
            // edge (x0,y0)-(x1,y1)
            if ((y0 <= y && y < y1) || (y1 <= y && y < y0)) {
                int32_t x = x0 + (y - y0) * (x1 - x0) / (y1 - y0);
                if (x < xl) xl = x;
                if (x > xr) xr = x;
            }
            // edge (x1,y1)-(x2,y2)
            if ((y1 <= y && y < y2) || (y2 <= y && y < y1)) {
                int32_t x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                if (x < xl) xl = x;
                if (x > xr) xr = x;
            }
            // edge (x2,y2)-(x0,y0)
            if ((y2 <= y && y < y0) || (y0 <= y && y < y2)) {
                int32_t x = x2 + (y - y2) * (x0 - x2) / (y0 - y2);
                if (x < xl) xl = x;
                if (x > xr) xr = x;
            }
            if (xl < 0) xl = 0;
            if (xr >= gfx_width()) xr = gfx_width() - 1;
            if (xl <= xr) {
                gfx_draw_line(xl, y, xr, y, color);
            }
        }
    }
}

/* Fallback untuk polygon umum (asumsi convex) */
void gfx_fill_polygon(const int32_t* points, uint32_t count, uint32_t color) {
    gfx_fill_convex_polygon(points, count, color);
}

/* ========== BEZIER CURVE (CUBIC) ========== */
void gfx_draw_bezier(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint32_t color) {
    // De Casteljau, iterasi 20 langkah
    const int steps = 20;
    int32_t prev_x = x0, prev_y = y0;
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;   /* jika floating diizinkan; alternatif fixed-point */
        fixed_t ft = fixed_from_float(t);   /* asumsi ada fixed_from_float, atau kita buat manual */
        fixed_t u = FIXED_ONE - ft;
        
        // Hitung koefisien Bernstein dalam fixed-point
        fixed_t b0 = fixed_mul(fixed_mul(u, u), u);
        fixed_t b1 = fixed_mul(fixed_mul(fixed_mul(u, u), ft), 3 << 16);
        fixed_t b2 = fixed_mul(fixed_mul(fixed_mul(u, ft), ft), 3 << 16);
        fixed_t b3 = fixed_mul(fixed_mul(ft, ft), ft);
        
        int32_t x = fixed_to_int(
            fixed_add(
                fixed_add(
                    fixed_add(
                        fixed_mul(b0, fixed_from_int(x0)),
                        fixed_mul(b1, fixed_from_int(x1))),
                    fixed_mul(b2, fixed_from_int(x2))),
                fixed_mul(b3, fixed_from_int(x3))
            )
        );
        int32_t y = fixed_to_int(
            fixed_add(
                fixed_add(
                    fixed_add(
                        fixed_mul(b0, fixed_from_int(y0)),
                        fixed_mul(b1, fixed_from_int(y1))),
                    fixed_mul(b2, fixed_from_int(y2))),
                fixed_mul(b3, fixed_from_int(y3))
            )
        );
        gfx_draw_line(prev_x, prev_y, x, y, color);
        prev_x = x; prev_y = y;
    }
}

/* ========== ARC (BAGIAN LINGKARAN) ========== */
/* Sudut dalam derajat, 0 = kanan, 90 = atas, dst */
void gfx_draw_arc(int32_t xc, int32_t yc, int32_t r, int32_t start_deg, int32_t end_deg, uint32_t color) {
    // Normalisasi sudut
    while (start_deg < 0) start_deg += 360;
    while (end_deg < 0) end_deg += 360;
    start_deg %= 360;
    end_deg %= 360;
    if (start_deg == end_deg) return;
    if (end_deg < start_deg) end_deg += 360;
    
    int steps = r * 2;  // resolusi sesuai jari-jari
    if (steps < 20) steps = 20;
    
    int32_t prev_x = -1, prev_y = -1;
    for (int i = 0; i <= steps; i++) {
        int deg = start_deg + (end_deg - start_deg) * i / steps;
        deg %= 360;
        fixed_t rad = fixed_from_int(deg);
        rad = fixed_mul(rad, 0x6487E); // deg2rad: pi/180 = 0.0174533 * 65536 = 1143. -> 0x477??
        // nilai tepat: pi/180 = 0.00872664626 * 65536 = 571.8? hitung ulang.
        // Lebih mudah: deg2rad = deg * pi/180, pi/180 = 0.01745329252 * 65536 = 1143. (0x477)
        rad = fixed_div(fixed_mul(fixed_from_int(deg), 0x3243F), 180 << 16); // (deg * pi) / 180
        int32_t x = xc + fixed_to_int(fixed_mul(fixed_from_int(r), fixed_cos(rad)));
        int32_t y = yc - fixed_to_int(fixed_mul(fixed_from_int(r), fixed_sin(rad)));
        
        if (i > 0) {
            gfx_draw_line(prev_x, prev_y, x, y, color);
        }
        prev_x = x; prev_y = y;
    }
}

void gfx_fill_arc(int32_t xc, int32_t yc, int32_t r, int32_t start_deg, int32_t end_deg, uint32_t color) {
    // Sederhana: gambar garis dari pusat ke setiap titik arc
    while (start_deg < 0) start_deg += 360;
    while (end_deg < 0) end_deg += 360;
    start_deg %= 360;
    end_deg %= 360;
    if (start_deg == end_deg) return;
    if (end_deg < start_deg) end_deg += 360;
    
    int steps = r * 2;
    if (steps < 20) steps = 20;
    
    for (int i = 0; i <= steps; i++) {
        int deg = start_deg + (end_deg - start_deg) * i / steps;
        deg %= 360;
        fixed_t rad = fixed_div(fixed_mul(fixed_from_int(deg), 0x3243F), 180 << 16);
        int32_t x = xc + fixed_to_int(fixed_mul(fixed_from_int(r), fixed_cos(rad)));
        int32_t y = yc - fixed_to_int(fixed_mul(fixed_from_int(r), fixed_sin(rad)));
        gfx_draw_line(xc, yc, x, y, color);
    }
    gfx_draw_arc(xc, yc, r, start_deg, end_deg, color);
}

/* ========== ELLIPSE ========== */
void gfx_draw_ellipse(int32_t xc, int32_t yc, int32_t a, int32_t b, uint32_t color) {
    // Midpoint ellipse algorithm
    int32_t x = 0, y = b;
    int32_t a2 = a * a, b2 = b * b;
    int32_t x2 = x * x, y2 = y * y;
    int32_t d1 = b2 - a2 * b + a2 / 4;
    
    while (a2 * y > b2 * x) {
        gfx_putpixel(xc + x, yc + y, color);
        gfx_putpixel(xc - x, yc + y, color);
        gfx_putpixel(xc + x, yc - y, color);
        gfx_putpixel(xc - x, yc - y, color);
        
        if (d1 < 0) {
            d1 += b2 * (2 * x + 3);
        } else {
            d1 += b2 * (2 * x + 3) + a2 * (-2 * y + 2);
            y--;
        }
        x++;
    }
    
    int32_t d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        gfx_putpixel(xc + x, yc + y, color);
        gfx_putpixel(xc - x, yc + y, color);
        gfx_putpixel(xc + x, yc - y, color);
        gfx_putpixel(xc - x, yc - y, color);
        
        if (d2 > 0) {
            d2 += a2 * (-2 * y + 3);
        } else {
            d2 += b2 * (2 * x + 2) + a2 * (-2 * y + 3);
            x++;
        }
        y--;
    }
}

void gfx_fill_ellipse(int32_t xc, int32_t yc, int32_t a, int32_t b, uint32_t color) {
    // Scanline fill untuk ellipse
    for (int32_t y = -b; y <= b; y++) {
        int32_t dy = y * y;
        if (dy > b * b) continue;
        int32_t x = (int32_t)((float)a * sqrtf(1.0f - (float)dy / (float)(b * b)));
        // Alternatif fixed-point sqrt
        fixed_t fy = fixed_from_int(y);
        fixed_t fb = fixed_from_int(b);
        fixed_t ratio = fixed_div(fixed_mul(fy, fy), fixed_mul(fb, fb));
        fixed_t fx = fixed_mul(fixed_from_int(a), fixed_sqrt(FIXED_ONE - ratio));
        int32_t xw = fixed_to_int(fx);
        gfx_draw_line(xc - xw, yc + y, xc + xw, yc + y, color);
    }
}
