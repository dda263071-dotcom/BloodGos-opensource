/**************************************************************
 * PS/2 Keyboard Driver - BloodG OS
 * Complete keyboard driver with scancode translation
 **************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "string.h"

// Keyboard ports
#define KEYBOARD_DATA   0x60
#define KEYBOARD_STATUS 0x64
#define KEYBOARD_CMD    0x64

// Keyboard commands
#define KEYBOARD_CMD_ENABLE  0xAE
#define KEYBOARD_CMD_DISABLE 0xAD
#define KEYBOARD_CMD_RESET   0xFF

// Status register bits
#define KEYBOARD_STATUS_OUTPUT_FULL 0x01
#define KEYBOARD_STATUS_INPUT_FULL  0x02

// Special keys
#define KEY_ENTER     0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_SPACE     0x39
#define KEY_ESC       0x01
#define KEY_CAPS_LOCK 0x3A
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_CTRL      0x1D
#define KEY_ALT       0x38
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44

// Keyboard state
static struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool num_lock;
    bool scroll_lock;
} keyboard_state = {0};

// Scancode translation tables
static const char scancode_normal[] = {
    0,   0x1B, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

static const char scancode_shift[] = {
    0,   0x1B, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

// Wait for keyboard controller to be ready for writing
static void keyboard_wait_write(void) {
    while (inb(KEYBOARD_STATUS) & KEYBOARD_STATUS_INPUT_FULL) {
        io_wait();
    }
}

// Wait for keyboard controller to have data
static void keyboard_wait_read(void) {
    while (!(inb(KEYBOARD_STATUS) & KEYBOARD_STATUS_OUTPUT_FULL)) {
        io_wait();
    }
}

// Send command to keyboard controller
static void keyboard_send_cmd(uint8_t cmd) {
    keyboard_wait_write();
    outb(KEYBOARD_CMD, cmd);
}

// Send data to keyboard
static void keyboard_send_data(uint8_t data) {
    keyboard_wait_write();
    outb(KEYBOARD_DATA, data);
}

// Initialize keyboard
bool keyboard_init(void) {
    print_string("Initializing PS/2 keyboard...\n");
    
    // Enable keyboard
    keyboard_send_cmd(KEYBOARD_CMD_ENABLE);
    
    // Test keyboard controller
    keyboard_send_cmd(0xAA);  // Self-test
    keyboard_wait_read();
    uint8_t test_result = inb(KEYBOARD_DATA);
    
    if (test_result != 0x55) {
        print_string("Keyboard: Controller test failed\n");
        return false;
    }
    
    // Reset keyboard
    keyboard_send_data(KEYBOARD_CMD_RESET);
    keyboard_wait_read();
    
    // Read reset response
    uint8_t response = inb(KEYBOARD_DATA);
    if (response != 0xFA) {
        print_string("Keyboard: Reset failed\n");
        return false;
    }
    
    // Wait for BAT completion
    keyboard_wait_read();
    response = inb(KEYBOARD_DATA);
    if (response != 0xAA) {
        print_string("Keyboard: BAT failed\n");
        return false;
    }
    
    // Enable scanning
    keyboard_send_data(0xF4);  // Enable scanning
    keyboard_wait_read();
    response = inb(KEYBOARD_DATA);
    
    if (response != 0xFA) {
        print_string("Keyboard: Failed to enable scanning\n");
        return false;
    }
    
    print_string("Keyboard: Initialized successfully\n");
    return true;
}

// Handle key press/release
static char keyboard_handle_scancode(uint8_t scancode) {
    static bool extended = false;
    static bool released = false;
    
    // Handle special prefix bytes
    if (scancode == 0xE0) {
        extended = true;
        return 0;
    }
    
    if (scancode == 0xF0) {
        released = true;
        return 0;
    }
    
    // Handle key release
    if (released) {
        released = false;
        
        // Update modifier states
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            keyboard_state.shift_pressed = false;
        } else if (scancode == KEY_CTRL) {
            keyboard_state.ctrl_pressed = false;
        } else if (scancode == KEY_ALT) {
            keyboard_state.alt_pressed = false;
        }
        
        return 0;
    }
    
    // Handle key press
    char key = 0;
    
    // Update modifier states
    if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
        keyboard_state.shift_pressed = true;
    } else if (scancode == KEY_CTRL) {
        keyboard_state.ctrl_pressed = true;
    } else if (scancode == KEY_ALT) {
        keyboard_state.alt_pressed = true;
    } else if (scancode == KEY_CAPS_LOCK) {
        keyboard_state.caps_lock = !keyboard_state.caps_lock;
    } else if (scancode == KEY_ENTER) {
        key = '\n';
    } else if (scancode == KEY_BACKSPACE) {
        key = '\b';
    } else if (scancode == KEY_SPACE) {
        key = ' ';
    } else if (scancode == KEY_ESC) {
        key = 0x1B;  // Escape character
    } else if (scancode >= 0x02 && scancode < 0x36) {
        // Regular key
        bool shift = keyboard_state.shift_pressed;
        bool caps = keyboard_state.caps_lock;
        
        if (scancode >= 0x10 && scancode <= 0x19) {  // Letters
            if (caps) {
                shift = !shift;
            }
        }
        
        if (shift) {
            key = scancode_shift[scancode];
        } else {
            key = scancode_normal[scancode];
        }
    }
    
    extended = false;
    return key;
}

// Get keyboard status
bool keyboard_has_data(void) {
    return (inb(KEYBOARD_STATUS) & KEYBOARD_STATUS_OUTPUT_FULL) != 0;
}

// Read character from keyboard (non-blocking)
char keyboard_read_char(void) {
    if (!keyboard_has_data()) {
        return 0;
    }
    
    uint8_t scancode = inb(KEYBOARD_DATA);
    return keyboard_handle_scancode(scancode);
}

// Read character from keyboard (blocking)
char keyboard_getchar(void) {
    while (!keyboard_has_data()) {
        // Wait for key press
    }
    return keyboard_read_char();
}

// Read string from keyboard (simple implementation)
void keyboard_read_string(char* buffer, size_t max_len) {
    size_t pos = 0;
    
    while (pos < max_len - 1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            buffer[pos] = '\0';
            print_string("\n");
            return;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                print_string("\b \b");
            }
        } else if (c >= 32 && c <= 126) {  // Printable characters
            buffer[pos++] = c;
            terminal_putchar(c);
        }
        // Ignore other characters
    }
    
    buffer[pos] = '\0';
}

// Get keyboard state
void keyboard_get_state(bool* shift, bool* ctrl, bool* alt, bool* caps) {
    if (shift) *shift = keyboard_state.shift_pressed;
    if (ctrl)  *ctrl = keyboard_state.ctrl_pressed;
    if (alt)   *alt = keyboard_state.alt_pressed;
    if (caps)  *caps = keyboard_state.caps_lock;
}
