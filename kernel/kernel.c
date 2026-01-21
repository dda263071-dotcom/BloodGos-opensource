/**************************************************************
 * BloodG OS Main Kernel with Filesystem Support
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// VGA constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((uint16_t*)0xB8000)

// Terminal state
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

// Keyboard buffer
#define INPUT_BUFFER_SIZE 256
static char input_buffer[INPUT_BUFFER_SIZE];
static size_t buffer_index = 0;

// Filesystem status
static bool filesystem_ready = false;

// Color enum
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

// Function declarations
void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_writestring(const char* str);
void terminal_clear(void);
void print_string(const char* str);
void keyboard_init(void);
void keyboard_handler(void);
void process_command(const char* cmd);

// Command functions
void help_command(void);
void clear_command(void);
void echo_command(const char* args);
void reboot_command(void);
void shutdown_command(void);
void ver_command(void);
void mem_command(void);
void about_command(void);
void ls_command(const char* args);
void cat_command(const char* args);

// External functions
extern void loading_show(void);
extern uint8_t inb(uint16_t port);
extern void outb(uint16_t port, uint8_t value);
extern void fat12_list_directory(void);
extern bool fat12_read_file(const char* filename, uint8_t* buffer, uint32_t max_size);
extern bool ata_init(void);
extern bool fat12_init(void);

// Command structure
struct command {
    const char* name;
    const char* description;
    void (*function)(const char* args);
};

// Command table with filesystem commands
static struct command commands[] = {
    {"help", "Show all commands", (void(*)(const char*))help_command},
    {"clear", "Clear screen", (void(*)(const char*))clear_command},
    {"cls", "Clear screen", (void(*)(const char*))clear_command},
    {"echo", "Print text", echo_command},
    {"reboot", "Restart system", (void(*)(const char*))reboot_command},
    {"shutdown", "Power off", (void(*)(const char*))shutdown_command},
    {"ver", "Show version", (void(*)(const char*))ver_command},
    {"mem", "Memory info", (void(*)(const char*))mem_command},
    {"about", "About BloodG OS", (void(*)(const char*))about_command},
    {"ls", "List directory", ls_command},
    {"dir", "List directory", ls_command},
    {"cat", "Show file contents", cat_command},
    {"type", "Show file contents", cat_command},
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

// Terminal functions (unchanged from your original, but included for completeness)
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | (uint16_t)color << 8;
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

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
            // Scroll implementation would go here
        }
        return;
    }
    
    if (c == '\b') {  // Backspace
        if (terminal_column > 0) {
            terminal_column--;
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = vga_entry(' ', terminal_color);
        return;
    }
    
    const size_t index = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_writestring(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

void print_string(const char* str) {
    terminal_writestring(str);
}

void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
}

// String utilities
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Command implementations
void help_command(void) {
    print_string("\nAvailable commands:\n");
    print_string("===================\n");
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        print_string("  ");
        print_string(commands[i].name);
        
        // Align descriptions
        size_t name_len = strlen(commands[i].name);
        while (name_len++ < 10) print_string(" ");
        
        print_string(" - ");
        print_string(commands[i].description);
        print_string("\n");
    }
}

void clear_command(void) {
    terminal_clear();
}

void echo_command(const char* args) {
    if (args) {
        print_string(args);
    }
    print_string("\n");
}

void reboot_command(void) {
    print_string("Rebooting...\n");
    extern void reboot(void);
    reboot();
}

void shutdown_command(void) {
    print_string("Shutting down...\n");
    extern void shutdown(void);
    shutdown();
}

void ver_command(void) {
    print_string("\n=== BloodG OS v2.1 ===\n");
    print_string("Version: 2.1.0\n");
    print_string("Build: Filesystem Edition\n");
    print_string("Features: FAT12, ATA PIO, CLI\n");
    print_string("Status: Experimental\n");
}

void mem_command(void) {
    print_string("\nMemory Information:\n");
    print_string("===================\n");
    print_string("Kernel: 0x1000-0x8FFF (32KB)\n");
    print_string("Stack: 0x90000-0x93FFF (16KB)\n");
    print_string("Video: 0xB8000-0xB8FA0 (4KB)\n");
    print_string("Available: 0x100000+ (15MB+)\n");
}

void about_command(void) {
    print_string("\n");
    print_string("╔══════════════════════════════════════╗\n");
    print_string("║         BLOODG OS v2.1              ║\n");
    print_string("║    Complete Operating System        ║\n");
    print_string("║  with FAT12 Filesystem Support      ║\n");
    print_string("║    Built for Education & Research   ║\n");
    print_string("╚══════════════════════════════════════╝\n");
}

// Filesystem commands
void ls_command(const char* args) {
    (void)args; // Not using args for now
    
    print_string("\nDirectory Listing:\n");
    print_string("==================\n");
    
    if (!filesystem_ready) {
        print_string("Filesystem not initialized.\n");
        print_string("Trying to initialize...\n");
        
        if (!ata_init() || !fat12_init()) {
            print_string("Failed to initialize filesystem.\n");
            print_string("Make sure a FAT12 disk is present.\n");
            return;
        }
        filesystem_ready = true;
    }
    
    fat12_list_directory();
    print_string("\n");
}

void cat_command(const char* args) {
    if (!args || !args[0]) {
        print_string("Usage: cat <filename>\n");
        return;
    }
    
    if (!filesystem_ready) {
        print_string("Filesystem not ready. Use 'ls' first.\n");
        return;
    }
    
    uint8_t buffer[4096];  // 4KB buffer for files
    
    print_string("\n");
    print_string("File: ");
    print_string(args);
    print_string("\n");
    print_string("========================================\n");
    
    if (fat12_read_file(args, buffer, sizeof(buffer))) {
        // Print file contents
        for (int i = 0; i < sizeof(buffer) && buffer[i] != 0; i++) {
            terminal_putchar(buffer[i]);
        }
    } else {
        print_string("Error: Cannot read file '");
        print_string(args);
        print_string("'\n");
        print_string("File may not exist or is too large.\n");
    }
    print_string("\n");
}

// Keyboard handling (unchanged)
void keyboard_init(void) {
    outb(0x64, 0xAE);  // Enable keyboard
}

void keyboard_handler(void) {
    if (inb(0x64) & 0x01) {
        uint8_t scancode = inb(0x60);
        
        if (scancode == 0x1C) {  // Enter
            if (buffer_index > 0) {
                input_buffer[buffer_index] = '\0';
                print_string("\n");
                process_command(input_buffer);
                buffer_index = 0;
            }
            print_string("bloodg> ");
        } else if (scancode == 0x0E) {  // Backspace
            if (buffer_index > 0) {
                buffer_index--;
                input_buffer[buffer_index] = '\0';
                terminal_putchar('\b');
            }
        } else if (scancode < 0x80) {
            // Simple scancode to ASCII
            char* keymap = "1234567890-=qwertyuiop[]asdfghjkl;'`\\zxcvbnm,./";
            if (scancode >= 0x02 && scancode < 0x36) {
                char c = keymap[scancode - 0x02];
                if (buffer_index < INPUT_BUFFER_SIZE - 1) {
                    input_buffer[buffer_index++] = c;
                    terminal_putchar(c);
                }
            } else if (scancode == 0x39) {  // Space
                if (buffer_index < INPUT_BUFFER_SIZE - 1) {
                    input_buffer[buffer_index++] = ' ';
                    terminal_putchar(' ');
                }
            }
        }
    }
}

void process_command(const char* cmd) {
    if (!cmd || !cmd[0]) return;
    
    // Extract command name
    char cmd_name[32];
    int i = 0;
    while (cmd[i] != ' ' && cmd[i] != '\0' && i < 31) {
        cmd_name[i] = cmd[i];
        i++;
    }
    cmd_name[i] = '\0';
    
    // Extract arguments
    const char* args = cmd + i;
    while (*args == ' ') args++;
    
    // Find and execute command
    for (size_t j = 0; j < COMMAND_COUNT; j++) {
        if (strcmp(cmd_name, commands[j].name) == 0) {
            commands[j].function(args);
            return;
        }
    }
    
    print_string("Unknown command: '");
    print_string(cmd_name);
    print_string("'\nType 'help' for available commands.\n");
}

// Main kernel function
int kmain(void) {
    terminal_initialize();
    
    // Show loading screen
    loading_show();
    
    // Initialize hardware
    keyboard_init();
    
    // Show welcome message
    print_string("\n\n");
    print_string("╔══════════════════════════════════════╗\n");
    print_string("║      BLOODG OS v2.1 - READY         ║\n");
    print_string("║    Filesystem Edition               ║\n");
    print_string("╚══════════════════════════════════════╝\n");
    print_string("\n");
    print_string("Filesystem: ");
    
    // Try to initialize filesystem
    if (ata_init() && fat12_init()) {
        filesystem_ready = true;
        print_string("FAT12 (Ready)\n");
    } else {
        print_string("Not available\n");
    }
    
    print_string("Type 'help' for commands\n\n");
    print_string("bloodg> ");
    
    // Main loop
    while (1) {
        keyboard_handler();
        
        // Simple delay
        for (volatile int i = 0; i < 10000; i++);
    }
    
    return 0;
}
