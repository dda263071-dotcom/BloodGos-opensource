/* ga wajib jika error disini skip aja ada alternatifnya*/
#include <lib_ui/window/window.h>
#include <lib_ui/graphics/graphics.h>
#include <lib_ui/font/font.h>

static uint32_t titlebar_height = 24;
static uint32_t close_button_size = 16;

void wm_draw_titlebar(window_t* win) {
    if (!win || !(win->flags & WF_VISIBLE)) return;
    
    uint32_t bg_color = (win->flags & WF_ACTIVE) ? COLOR_BLUE : COLOR_DARK_GRAY;
    uint32_t fg_color = COLOR_WHITE;
    
    /* Background title bar */
    gfx_fill_rect(win->x, win->y, win->width, titlebar_height, bg_color);
    
    /* Border bawah title bar */
    gfx_draw_line(win->x, win->y + titlebar_height - 1,
                  win->x + win->width - 1, win->y + titlebar_height - 1,
                  COLOR_BLACK);
    
    /* Title text */
    if (win->title && title_font) {
        uint32_t text_y = win->y + (titlebar_height - title_font->height) / 2;
        uint32_t text_x = win->x + 4;
        font_draw_string(title_font, text_x, text_y, win->title, fg_color);
    }
    
    /* Close button */
    if (win->flags & WF_CLOSABLE) {
        uint32_t btn_x = win->x + win->width - close_button_size - 4;
        uint32_t btn_y = win->y + (titlebar_height - close_button_size) / 2;
        
        gfx_draw_rect(btn_x, btn_y, close_button_size, close_button_size, COLOR_WHITE);
        /* Tanda X */
        gfx_draw_line(btn_x + 2, btn_y + 2,
                      btn_x + close_button_size - 3, btn_y + close_button_size - 3,
                      COLOR_WHITE);
        gfx_draw_line(btn_x + close_button_size - 3, btn_y + 2,
                      btn_x + 2, btn_y + close_button_size - 3,
                      COLOR_WHITE);
    }
}
