#ifndef LIB_UI_WINDOW_H
#define LIB_UI_WINDOW_H

#include <libc/types.h>
#include <lib_ui/graphics/graphics.h>
#include <lib_ui/font/font.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
   Window flags
   ============================================================================ */
#define WF_VISIBLE      (1 << 0)
#define WF_RESIZABLE    (1 << 1)
#define WF_CLOSABLE     (1 << 2)
#define WF_MOVABLE      (1 << 3)
#define WF_ACTIVE       (1 << 4)

/* ============================================================================
   Window structure
   ============================================================================ */
typedef struct window {
    /* Geometry */
    uint32_t x, y;
    uint32_t width, height;
    uint32_t min_width, min_height;
    
    /* State */
    uint32_t flags;
    char*    title;
    
    /* Callback untuk menggambar konten window */
    void (*draw_content)(struct window* win, void* priv);
    void*   priv;   /* data pribadi untuk callback */
    
    /* Z‑order linked list */
    struct window* prev;
    struct window* next;
} window_t;

/* ============================================================================
   Global window manager state
   ============================================================================ */
void        wm_init(font_t* title_font);   /* font untuk title bar */
void        wm_set_desktop_color(uint32_t color);
font_t*     wm_get_title_font(void);

/* ============================================================================
   Window creation / destruction
   ============================================================================ */
window_t*   wm_create_window(uint32_t x, uint32_t y,
                             uint32_t w, uint32_t h,
                             const char* title);
void        wm_destroy_window(window_t* win);
void        wm_destroy_all(void);

/* ============================================================================
   Window manipulation
   ============================================================================ */
void        wm_move_window(window_t* win, uint32_t x, uint32_t y);
void        wm_resize_window(window_t* win, uint32_t w, uint32_t h);
void        wm_set_window_title(window_t* win, const char* title);
void        wm_show_window(window_t* win);
void        wm_hide_window(window_t* win);
void        wm_set_active_window(window_t* win);
window_t*   wm_get_active_window(void);
void        wm_move_to_front(window_t* win);

/* ============================================================================
   Rendering
   ============================================================================ */
void        wm_draw_all(void);              /* gambar semua window */
void        wm_draw_window(window_t* win);  /* gambar satu window */
void        wm_draw_titlebar(window_t* win);
void        wm_draw_content(window_t* win); /* panggil callback */

/* ============================================================================
   Mouse event handling (panggil dari driver)
   ============================================================================ */
void        wm_handle_mouse_move(uint32_t x, uint32_t y);
void        wm_handle_mouse_button(int button, int pressed);
int         wm_is_dragging(void);           /* apakah sedang drag window? */

/* ============================================================================
   Hit testing
   ============================================================================ */
window_t*   wm_get_window_at(uint32_t x, uint32_t y);
int         wm_is_in_titlebar(window_t* win, uint32_t x, uint32_t y);
int         wm_is_in_close_button(window_t* win, uint32_t x, uint32_t y);
int         wm_is_in_resize_handle(window_t* win, uint32_t x, uint32_t y);

#ifdef __cplusplus
}
#endif

#endif /* LIB_UI_WINDOW_H */
