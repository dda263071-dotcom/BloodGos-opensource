#include <lib_ui/window/window.h>
#include <libc/string.h>
#include <libc/malloc.h>

/* ============================================================================
   Global state
   ============================================================================ */
static window_t* window_list_head = NULL;
static window_t* active_window = NULL;
static window_t* drag_window = NULL;
static int32_t   drag_start_x, drag_start_y;
static uint32_t  drag_win_x, drag_win_y;

static font_t*   title_font = NULL;
static uint32_t  desktop_color = COLOR_DARK_GRAY;
static uint32_t  titlebar_height = 24;
static uint32_t  close_button_size = 16;

/* ============================================================================
   Init
   ============================================================================ */
void wm_init(font_t* font) {
    title_font = font;
    window_list_head = NULL;
    active_window = NULL;
    drag_window = NULL;
}

void wm_set_desktop_color(uint32_t color) {
    desktop_color = color;
}

font_t* wm_get_title_font(void) {
    return title_font;
}

uint32_t wm_get_titlebar_height(void) {
    return titlebar_height;
}

/* ============================================================================
   Window list management
   ============================================================================ */
static void wm_add_to_list(window_t* win) {
    win->next = window_list_head;
    win->prev = NULL;
    if (window_list_head) window_list_head->prev = win;
    window_list_head = win;
}

static void wm_remove_from_list(window_t* win) {
    if (win->prev) win->prev->next = win->next;
    if (win->next) win->next->prev = win->prev;
    if (window_list_head == win) window_list_head = win->next;
    win->prev = win->next = NULL;
}

/* ============================================================================
   Widget array management
   ============================================================================ */
static int wm_ensure_capacity(window_t* win, uint32_t capacity) {
    if (capacity <= win->widget_capacity) return 0;
    
    uint32_t new_cap = win->widget_capacity == 0 ? 4 : win->widget_capacity * 2;
    if (new_cap < capacity) new_cap = capacity;
    
    widget_t** new_widgets = realloc(win->widgets, new_cap * sizeof(widget_t*));
    if (!new_widgets) return -1;
    
    win->widgets = new_widgets;
    win->widget_capacity = new_cap;
    return 0;
}

/* ============================================================================
   Window creation
   ============================================================================ */
window_t* wm_create_window(uint32_t x, uint32_t y,
                           uint32_t w, uint32_t h,
                           const char* title) {
    window_t* win = malloc(sizeof(window_t));
    if (!win) return NULL;
    
    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;
    win->min_width = 64;
    win->min_height = 48;
    win->flags = WF_VISIBLE | WF_MOVABLE | WF_CLOSABLE;
    win->title = title ? strdup(title) : NULL;
    win->draw_content = NULL;
    win->priv = NULL;
    
    /* Widget array */
    win->widgets = NULL;
    win->widget_count = 0;
    win->widget_capacity = 0;
    
    win->prev = win->next = NULL;
    
    wm_add_to_list(win);
    wm_move_to_front(win);
    return win;
}

void wm_destroy_window(window_t* win) {
    if (!win) return;
    
    /* Hapus semua widget */
    wm_remove_all_widgets(win);
    if (win->widgets) free(win->widgets);
    
    if (active_window == win) active_window = NULL;
    if (drag_window == win) drag_window = NULL;
    
    wm_remove_from_list(win);
    if (win->title) free(win->title);
    free(win);
}

void wm_destroy_all(void) {
    while (window_list_head) {
        wm_destroy_window(window_list_head);
    }
}

/* ============================================================================
   Window manipulation
   ============================================================================ */
void wm_move_window(window_t* win, uint32_t x, uint32_t y) {
    if (!win) return;
    win->x = x;
    win->y = y;
}

void wm_resize_window(window_t* win, uint32_t w, uint32_t h) {
    if (!win) return;
    if (w < win->min_width) w = win->min_width;
    if (h < win->min_height) h = win->min_height;
    win->width = w;
    win->height = h;
}

void wm_set_window_title(window_t* win, const char* title) {
    if (!win) return;
    if (win->title) free(win->title);
    win->title = title ? strdup(title) : NULL;
}

void wm_show_window(window_t* win) {
    if (win) win->flags |= WF_VISIBLE;
}

void wm_hide_window(window_t* win) {
    if (win) win->flags &= ~WF_VISIBLE;
}

void wm_set_active_window(window_t* win) {
    if (!win) return;
    if (active_window) active_window->flags &= ~WF_ACTIVE;
    active_window = win;
    win->flags |= WF_ACTIVE;
    wm_move_to_front(win);
}

window_t* wm_get_active_window(void) {
    return active_window;
}

void wm_move_to_front(window_t* win) {
    if (!win || window_list_head == win) return;
    wm_remove_from_list(win);
    win->next = window_list_head;
    win->prev = NULL;
    if (window_list_head) window_list_head->prev = win;
    window_list_head = win;
}

/* ============================================================================
   Widget management
   ============================================================================ */
int wm_add_widget(window_t* win, widget_t* widget) {
    if (!win || !widget) return -1;
    
    if (wm_ensure_capacity(win, win->widget_count + 1) != 0)
        return -1;
    
    win->widgets[win->widget_count++] = widget;
    return 0;
}

int wm_remove_widget(window_t* win, widget_t* widget) {
    if (!win || !widget) return -1;
    
    for (uint32_t i = 0; i < win->widget_count; i++) {
        if (win->widgets[i] == widget) {
            /* Geser sisa array */
            for (uint32_t j = i; j < win->widget_count - 1; j++) {
                win->widgets[j] = win->widgets[j + 1];
            }
            win->widget_count--;
            return 0;
        }
    }
    return -1;
}

void wm_remove_all_widgets(window_t* win) {
    if (!win) return;
    win->widget_count = 0;
    /* Widget objects sendiri harus di-free oleh pemiliknya */
}

widget_t* wm_get_widget_at(window_t* win, uint32_t x, uint32_t y) {
    if (!win || !(win->flags & WF_VISIBLE)) return NULL;
    
    /* Cari dari widget terakhir (paling depan) */
    for (int32_t i = (int32_t)win->widget_count - 1; i >= 0; i--) {
        widget_t* w = win->widgets[i];
        if (!w || !w->visible) continue;
        
        /* Hitung posisi absolut widget */
        uint32_t abs_x = win->x + w->x;
        uint32_t abs_y = win->y + titlebar_height + w->y;
        
        if (x >= abs_x && x < abs_x + w->width &&
            y >= abs_y && y < abs_y + w->height) {
            return w;
        }
    }
    return NULL;
}

/* ============================================================================
   Rendering
   ============================================================================ */
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

void wm_draw_widgets(window_t* win) {
    if (!win || !(win->flags & WF_VISIBLE)) return;
    
    for (uint32_t i = 0; i < win->widget_count; i++) {
        widget_t* w = win->widgets[i];
        if (!w || !w->visible || !w->draw) continue;
        
        /* Simpan posisi relatif original */
        uint32_t orig_x = w->x;
        uint32_t orig_y = w->y;
        
        /* Konversi ke posisi absolut untuk rendering */
        w->x = win->x + orig_x;
        w->y = win->y + titlebar_height + orig_y;
        
        /* Panggil method draw widget */
        w->draw(w);
        
        /* Kembalikan posisi relatif */
        w->x = orig_x;
        w->y = orig_y;
    }
}

void wm_draw_content(window_t* win) {
    if (!win || !(win->flags & WF_VISIBLE)) return;
    
    /* Area konten window (di bawah title bar) */
    uint32_t content_x = win->x + 1;
    uint32_t content_y = win->y + titlebar_height + 1;
    uint32_t content_w = win->width - 2;
    uint32_t content_h = win->height - titlebar_height - 2;
    
    /* Gambar background konten */
    gfx_fill_rect(content_x, content_y, content_w, content_h, COLOR_WHITE);
    
    /* Callback custom jika ada */
    if (win->draw_content) {
        win->draw_content(win, win->priv);
    }
    
    /* Gambar semua widget */
    wm_draw_widgets(win);
}

void wm_draw_window(window_t* win) {
    if (!win || !(win->flags & WF_VISIBLE)) return;
    
    wm_draw_titlebar(win);
    wm_draw_content(win);
    
    /* Border luar window */
    gfx_draw_rect(win->x, win->y, win->width, win->height, COLOR_BLACK);
}

void wm_draw_all(void) {
    /* Clear desktop */
    gfx_clear(desktop_color);
    
    /* Cari tail (window paling belakang) */
    window_t* win = window_list_head;
    if (!win) return;
    
    while (win->next) win = win->next;
    
    /* Gambar dari belakang ke depan */
    while (win) {
        wm_draw_window(win);
        win = win->prev;
    }
}

/* ============================================================================
   Hit testing
   ============================================================================ */
window_t* wm_get_window_at(uint32_t x, uint32_t y) {
    window_t* win = window_list_head;
    while (win) {
        if ((win->flags & WF_VISIBLE) &&
            x >= win->x && x < win->x + win->width &&
            y >= win->y && y < win->y + win->height) {
            return win;
        }
        win = win->next;
    }
    return NULL;
}

int wm_is_in_titlebar(window_t* win, uint32_t x, uint32_t y) {
    if (!win) return 0;
    return (y >= win->y && y < win->y + titlebar_height &&
            x >= win->x && x < win->x + win->width);
}

int wm_is_in_close_button(window_t* win, uint32_t x, uint32_t y) {
    if (!win || !(win->flags & WF_CLOSABLE)) return 0;
    uint32_t btn_x = win->x + win->width - close_button_size - 4;
    uint32_t btn_y = win->y + (titlebar_height - close_button_size) / 2;
    return (x >= btn_x && x < btn_x + close_button_size &&
            y >= btn_y && y < btn_y + close_button_size);
}

int wm_is_in_resize_handle(window_t* win, uint32_t x, uint32_t y) {
    if (!win || !(win->flags & WF_RESIZABLE)) return 0;
    int handle_size = 8;
    return (x >= win->x + win->width - handle_size &&
            y >= win->y + win->height - handle_size);
}

/* ============================================================================
   Mouse event handling
   ============================================================================ */
static uint32_t last_mouse_x = 0, last_mouse_y = 0;

void wm_handle_mouse_move(uint32_t x, uint32_t y) {
    last_mouse_x = x;
    last_mouse_y = y;
    
    if (drag_window) {
        int32_t dx = x - drag_start_x;
        int32_t dy = y - drag_start_y;
        wm_move_window(drag_window, drag_win_x + dx, drag_win_y + dy);
    }
}

void wm_handle_mouse_button(uint32_t x, uint32_t y, int button, int pressed) {
    if (button != 0) return;  /* hanya tombol kiri */
    
    if (pressed) {
        window_t* win = wm_get_window_at(x, y);
        if (win) {
            wm_set_active_window(win);
            
            if (wm_is_in_close_button(win, x, y)) {
                wm_destroy_window(win);
            } else if (wm_is_in_titlebar(win, x, y) && (win->flags & WF_MOVABLE)) {
                drag_window = win;
                drag_start_x = x;
                drag_start_y = y;
                drag_win_x = win->x;
                drag_win_y = win->y;
            } else {
                /* Click pada widget di dalam window */
                widget_t* w = wm_get_widget_at(win, x, y);
                if (w && w->click) {
                    /* Hitung posisi relatif terhadap widget */
                    uint32_t rel_x = x - (win->x + w->x);
                    uint32_t rel_y = y - (win->y + titlebar_height + w->y);
                    w->click(w, rel_x, rel_y);
                }
            }
        }
    } else {
        /* Mouse button release */
        if (drag_window) {
            drag_window = NULL;
        }
    }
}

int wm_is_dragging(void) {
    return drag_window != NULL;
}
