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
    win->title = NULL;
    if (title) {
        win->title = strdup(title);
    }
    win->draw_content = NULL;
    win->priv = NULL;
    win->prev = win->next = NULL;
    
    wm_add_to_list(win);
    wm_move_to_front(win);
    return win;
}

void wm_destroy_window(window_t* win) {
    if (!win) return;
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
   Mouse event handling (dipanggil driver)
   ============================================================================ */
void wm_handle_mouse_move(uint32_t x, uint32_t y) {
    static uint32_t last_x = 0, last_y = 0;
    
    if (drag_window) {
        int32_t dx = x - drag_start_x;
        int32_t dy = y - drag_start_y;
        wm_move_window(drag_window, drag_win_x + dx, drag_win_y + dy);
        return;
    }
    last_x = x; last_y = y;
}

void wm_handle_mouse_button(int button, int pressed) {
    if (button != 0) return; /* hanya tombol kiri */
    
    uint32_t mouse_x, mouse_y;
  
}


void wm_handle_mouse_button(uint32_t x, uint32_t y, int button, int pressed) {
    if (button != 0) return;
    
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
            }
        }
    } else {
        if (drag_window) {
            /* selesai drag */
            drag_window = NULL;
        }
    }
}

int wm_is_dragging(void) {
    return drag_window != NULL;
}
