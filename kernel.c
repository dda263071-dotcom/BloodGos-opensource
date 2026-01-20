/**************************************************************
 * BloodG OS - Minimal x86 Operating System Kernel
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Hardware text mode color constants */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

/* VGA text buffer */
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

/* Terminal state */
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

/* Keyboard dengan state management */
#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static size_t buffer_index = 0;
static bool shift_pressed = false;
static bool caps_lock = false;

/* Command structure */
struct command {
    const char* name;
    const char* description;
    void (*function)(void);
};

/* Function prototypes */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
static inline uint16_t vga_entry(unsigned char uc, uint8_t color);
void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_newline(void);
void terminal_clear(void);
void print_string(const char* str);

/* Command functions */
void help_command(void);
void clear_command(void);
void echo_command(void);
void reboot_command(void);
void shutdown_command(void);
void ver_command(void);
void color_command(void);
void ls_command(void);
void time_command(void);
void date_command(void);
void calc_command(void);
void mem_command(void);
void exit_command(void);
void about_command(void);

/* Process command prototype */
void process_command(const char* cmd);

/* Command table */
struct command commands[] = {
    {"help", "Show all available commands", help_command},
    {"clear", "Clear the terminal screen", clear_command},
    {"cls", "Clear screen (alias for clear)", clear_command},
    {"echo", "Print text to the screen", echo_command},
    {"reboot", "Restart the system", reboot_command},
    {"shutdown", "Power off the system", shutdown_command},
    {"ver", "Show system version information", ver_command},
    {"color", "Change terminal text color (0-9)", color_command},
    {"ls", "List directories (placeholder)", ls_command},
    {"time", "Show current system time", time_command},
    {"date", "Show current system date", date_command},
    {"calc", "Simple calculator", calc_command},
    {"mem", "Show memory information", mem_command},
    {"exit", "Exit terminal session", exit_command},
    {"about", "Show information about BloodG OS", about_command},
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

/**************************************************************
 * I/O Port Functions
 **************************************************************/

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/**************************************************************
 * VGA Terminal Functions
 **************************************************************/

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void terminal_initialize(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_buffer = VGA_MEMORY;
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color) {
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_newline();
        return;
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        terminal_newline();
    }
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    size_t len = 0;
    while (data[len]) len++;
    terminal_write(data, len);
}

void print_string(const char* str) {
    terminal_writestring(str);
}

void terminal_newline(void) {
    terminal_column = 0;
    
    if (++terminal_row == VGA_HEIGHT) {
        // Scroll the screen up by one line
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                const size_t dest_index = (y - 1) * VGA_WIDTH + x;
                const size_t src_index = y * VGA_WIDTH + x;
                terminal_buffer[dest_index] = terminal_buffer[src_index];
            }
        }
        
        // Clear the last line
        terminal_row = VGA_HEIGHT - 1;
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, terminal_row);
        }
    }
}

void terminal_clear(void) {
    terminal_row = 0;
    terminal_column = 0;
    
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_putentryat(' ', terminal_color, x, y);
        }
    }
    
    terminal_row = 0;
    terminal_column = 0;
}

/**************************************************************
 * String Utility Functions
 **************************************************************/

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* a, const char* b) {
    while (*a && (*a == *b)) a++, b++;
    return *(const unsigned char*)a - *(const unsigned char*)b;
}

void memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;
    for (size_t i = 0; i < num; i++) p[i] = (unsigned char)value;
}

/**************************************************************
 * Simple Keyboard Driver (POLLING MODE)
 **************************************************************/

/* Keyboard scancode set 1 mapping - FIXED VERSION */
static const char kbd_ascii_lower[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char kbd_ascii_upper[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Simple keyboard polling - FIXED */
void poll_keyboard(void) {
    // Check if keyboard has data
    if (!(inb(0x64) & 0x01)) return;
    
    uint8_t scancode = inb(0x60);
    
    // Key release (high bit set)
    if (scancode & 0x80) {
        uint8_t released_key = scancode & 0x7F;
        
        // Handle shift release
        if (released_key == 0x2A || released_key == 0x36) {
            shift_pressed = false;
        }
        return;
    }
    
    // Key press
    switch (scancode) {
        case 0x2A:  // Left Shift
        case 0x36:  // Right Shift
            shift_pressed = true;
            break;
            
        case 0x3A:  // Caps Lock
            caps_lock = !caps_lock;
            // Bisa tambahkan LED feedback di sini
            break;
            
        case 0x1C:  // Enter
            if (buffer_index > 0) {
                input_buffer[buffer_index] = '\0';
                print_string("\n");
                process_command(input_buffer);
                memset(input_buffer, 0, INPUT_BUFFER_SIZE);
                buffer_index = 0;
            }
            print_string("root~~bloodg:~# ");
            break;
            
        case 0x0E:  // Backspace
            if (buffer_index > 0) {
                buffer_index--;
                input_buffer[buffer_index] = '\0';
                
                // Move cursor back visually - FIXED
                if (terminal_column > 0) {
                    terminal_column--;
                } else if (terminal_row > 0) {
                    terminal_row--;
                    terminal_column = VGA_WIDTH - 1;
                }
                terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            }
            break;
            
        default:
            // Convert scancode to ASCII
            if (scancode < 128) {
                char c;
                
                if (shift_pressed || caps_lock) {
                    c = kbd_ascii_upper[scancode];
                    // Jika caps_lock aktif TAPI shift ditekan, gunakan lowercase
                    if (caps_lock && shift_pressed && c >= 'A' && c <= 'Z') {
                        c += 32;  // ke lowercase
                    }
                } else {
                    c = kbd_ascii_lower[scancode];
                }
                
                if (c && buffer_index < INPUT_BUFFER_SIZE - 1) {
                    input_buffer[buffer_index++] = c;
                    terminal_putchar(c);
                }
            }
            break;
    }
}

/**************************************************************
 * Basic Command Implementations
 **************************************************************/

void help_command(void) {
    print_string("\nAvailable commands:\n");
    print_string("===================\n");
    
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        print_string("  ");
        print_string(commands[i].name);
        
        size_t name_len = 0;
        const char* name_ptr = commands[i].name;
        while (*name_ptr++) name_len++;
        
        if (name_len < 8) {
            for (size_t j = name_len; j < 8; j++) {
                print_string(" ");
            }
        }
        
        print_string(" - ");
        print_string(commands[i].description);
        print_string("\n");
    }
    
    print_string("\n");
}

void clear_command(void) {
    terminal_clear();
}

void echo_command(void) {
    // Skip past "echo " in the input buffer
    char* args = input_buffer + 5;
    if (*args) {
        print_string(args);
    }
    print_string("\n");
}

void reboot_command(void) {
    print_string("Reboot not implemented in polling mode.\n");
}

void shutdown_command(void) {
    print_string("Shutdown not implemented in polling mode.\n");
    print_string("To exit in QEMU: Press Ctrl+Alt+Del\n");
}

void ver_command(void) {
    print_string("\n=== BloodG OS ===\n");
    print_string("Version: 0.1        \n");
    print_string("Build: Minimal x86 Kernel\n");
    print_string("Status: Running in 32-bit\n");
    print_string("===================\n");
}

void color_command(void) {
    print_string("\nTerminal colors not implemented yet.\n");
    print_string("Default: Light Green on Black\n");
}

void ls_command(void) {
    print_string("\nDirectory listing placeholder\n");
    print_string("(No filesystem implemented)\n");
}

void time_command(void) {
    print_string("System time: Not implemented\n");
}

void date_command(void) {
    print_string("System date: Not implemented\n");
}

void calc_command(void) {
    print_string("\nCalculator placeholder\n");
}

void mem_command(void) {
    print_string("\nMemory: 64MB (simulated)\n");
    print_string("Kernel: ~20KB\n");
}

void exit_command(void) {
    print_string("Cannot exit kernel. Type 'reboot' to restart.\n");
}

void about_command(void) {
    print_string("\n=== BLOODG OS v0.1 ===\n");
    print_string("Minimal x86 Educational Kernel\n");
    print_string("Running in safe polling mode\n");
    print_string("Features: VGA text, keyboard input\n");
    print_string("Commands: 15 available\n");
    print_string("===========================\n");
}

/**************************************************************
 * Command Processing
 **************************************************************/

void process_command(const char* cmd) {
    if (!cmd || !cmd[0]) return;
    
    // Skip leading spaces
    while (*cmd == ' ') cmd++;
    
    // Extract command name
    char cmd_name[32];
    size_t i = 0;
    while (cmd[i] != ' ' && cmd[i] != '\0' && i < 31) {
        cmd_name[i] = cmd[i];
        i++;
    }
    cmd_name[i] = '\0';
    
    // Find and execute command
    bool found = false;
    for (size_t j = 0; j < COMMAND_COUNT; j++) {
        const char* cmd_ptr = commands[j].name;
        const char* name_ptr = cmd_name;
        bool match = true;
        
        while (*cmd_ptr && *name_ptr) {
            if (*cmd_ptr != *name_ptr) {
                match = false;
                break;
            }
            cmd_ptr++;
            name_ptr++;
        }
        
        if (match && *cmd_ptr == '\0' && *name_ptr == '\0') {
            commands[j].function();
            found = true;
            break;
        }
    }
    
    if (!found) {
        print_string("Unknown command: '");
        print_string(cmd_name);
        print_string("'\nType 'help' for available commands.\n");
    }
}

/**************************************************************
 * Main Kernel Function
 **************************************************************/

int kmain(void) {
    // Initialize terminal
    terminal_initialize();
    
    // Show boot banner
    print_string("========================================\n");
    print_string("           bloodG                       \n");
    print_string("                   not found            \n");
    print_string("========================================\n");
    print_string("System initializing...\n");
    print_string("VGA: READY\n");
    print_string("Keyboard: POLLING MODE\n");
    print_string("Commands: LOADED\n");
    print_string("========================================\n\n");
    
    print_string("System ready. Type 'help' for commands.\n\n");
    print_string("root~~bloodg:~# ");
    
    // Main loop dengan polling
    while (1) {
        poll_keyboard();
        
        // Small delay to reduce CPU usage
        for (volatile int i = 0; i < 10000; i++) {}
    }
    
    return 0;
}
