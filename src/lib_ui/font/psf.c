#include <lib_ui/font/font.h>
#include <libc/string.h>
#include <libc/malloc.h>

/* PSF1 magic */
#define PSF1_MAGIC0     0x36
#define PSF1_MAGIC1     0x04
#define PSF1_MODE512    0x01
#define PSF1_MODEHASTAB 0x02
#define PSF1_MODESEQ    0x04

/* PSF2 magic */
#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2     0x4a
#define PSF2_MAGIC3     0x86

int font_load_psf(font_t* font, const void* data) {
    if (!font || !data) return -1;

    const uint8_t* p = (const uint8_t*)data;

    /* Cek PSF1 */
    if (p[0] == PSF1_MAGIC0 && p[1] == PSF1_MAGIC1) {
        psf1_header_t* hdr = (psf1_header_t*)data;
        font->is_psf2 = 0;
        font->height = hdr->charsize;
        font->width = 8;  /* PSF1 selalu 8 pixel lebar */
        font->bytes_per_glyph = hdr->charsize; /* 8xN, 1 byte per scanline */
        
        uint32_t num_glyphs = (hdr->mode & PSF1_MODE512) ? 512 : 256;
        uint32_t total_bytes = num_glyphs * font->bytes_per_glyph;
        
        font->glyph_data = malloc(total_bytes);
        if (!font->glyph_data) return -1;
        
        memcpy(font->glyph_data, p + sizeof(psf1_header_t), total_bytes);
        return 0;
    }

    /* Cek PSF2 */
    if (p[0] == PSF2_MAGIC0 && p[1] == PSF2_MAGIC1 &&
        p[2] == PSF2_MAGIC2 && p[3] == PSF2_MAGIC3) {
        psf2_header_t* hdr = (psf2_header_t*)data;
        font->is_psf2 = 1;
        font->width = hdr->width;
        font->height = hdr->height;
        font->bytes_per_glyph = hdr->bytesperglyph;
        
        uint32_t total_bytes = hdr->numglyph * hdr->bytesperglyph;
        font->glyph_data = malloc(total_bytes);
        if (!font->glyph_data) return -1;
        
        memcpy(font->glyph_data, p + hdr->headersize, total_bytes);
        return 0;
    }

    return -1;  /* Format tidak dikenal */
}

void font_free(font_t* font) {
    if (font && font->glyph_data) {
        free(font->glyph_data);
        font->glyph_data = NULL;
    }
}
