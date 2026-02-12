#ifndef LIB_UI_FONT_H
#define LIB_UI_FONT_H

#include <libc/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
   Font structures
   ============================================================================ */

/* PSF1 font header (32 byte) */
typedef struct {
    uint16_t magic;
    uint8_t  mode;
    uint8_t  charsize;
} psf1_header_t;

/* PSF2 font header */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t headersize;
    uint32_t flags;
    uint32_t numglyph;
    uint32_t bytesperglyph;
    uint32_t height;
    uint32_t width;
} psf2_header_t;

/* Generic font structure */
typedef struct {
    uint32_t width;      /* karakter width dalam pixel */
    uint32_t height;     /* karakter height dalam pixel */
    uint32_t bytes_per_glyph;
    uint8_t* glyph_data; /* array glyph data */
    uint8_t  is_psf2;    /* 0 = PSF1, 1 = PSF2 */
} font_t;

/* ============================================================================
   Font loading
   ============================================================================ */

/* Load PSF font from memory (ROM, filesystem, etc.) */
int  font_load_psf(font_t* font, const void* psf_data);

/* Use built-in 8x16 bitmap font (always available) */
void font_use_builtin(font_t* font);

/* Free font glyph data (if allocated) */
void font_free(font_t* font);

/* ============================================================================
   Rendering
   ============================================================================ */

/* Draw single character at (x,y) with color */
void font_draw_char(font_t* font, uint32_t x, uint32_t y, char c, uint32_t color);

/* Draw null-terminated string */
void font_draw_string(font_t* font, uint32_t x, uint32_t y, const char* str, uint32_t color);

/* Draw string with background color */
void font_draw_string_bg(font_t* font, uint32_t x, uint32_t y, const char* str, uint32_t fg, uint32_t bg);

/* Get width of string in pixels */
uint32_t font_string_width(font_t* font, const char* str);

#ifdef __cplusplus
}
#endif

#endif /* LIB_UI_FONT_H */
