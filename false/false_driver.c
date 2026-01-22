/**************************************************************
 * Driver Error Handler - BloodG OS
 * Handles driver initialization and runtime errors
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// VGA access
#define VGA_MEMORY ((uint16_t*)0xB8000)
#define VGA_WIDTH 80

static void write_string(const char* str, uint8_t color, size_t x, size_t y) {
    for (size_t i = 0; str[i] && x + i < VGA_WIDTH; i++) {
        VGA_MEMORY[y * VGA_WIDTH + x + i] = (uint16_t)str[i] | ((uint16_t)color << 8);
    }
}

// Driver error types
typedef enum {
    DRIVER_INIT_FAILED,
    DRIVER_TIMEOUT,
    DRIVER_HARDWARE_ERROR,
    DRIVER_MEMORY_ERROR,
    DRIVER_IO_ERROR,
    DRIVER_NOT_FOUND,
    DRIVER_VERSION_MISMATCH
} driver_error_t;

// Driver names
static const char* driver_names[] = {
    "ATA Disk Driver",
    "Keyboard Driver",
    "VGA Driver",
    "Timer Driver",
    "Serial Driver",
    "PIC Driver",
    "FAT12 Filesystem"
};

// Driver error handler
void driver_error(driver_error_t error_type, int driver_id, const char* extra_info) {
    static bool error_displayed = false;
    
    // Only show first error to avoid screen flooding
    if (error_displayed) return;
    error_displayed = true;
    
    // Save original screen
    uint16_t saved_screen[VGA_WIDTH * 25];
    for (int i = 0; i < VGA_WIDTH * 25; i++) {
        saved_screen[i] = VGA_MEMORY[i];
    }
    
    // Clear area for error message
    uint8_t error_color = 0x4E; // Yellow text on red background
    
    // Draw error box
    for (int y = 8; y <= 16; y++) {
        for (int x = 10; x <= 69; x++) {
            if (y == 8 || y == 16 || x == 10 || x == 69) {
                // Border
                if ((y == 8 && x == 10) || (y == 8 && x == 69) ||
                    (y == 16 && x == 10) || (y == 16 && x == 69)) {
                    // Corners
                    char corner = '+';
                    VGA_MEMORY[y * VGA_WIDTH + x] = (uint16_t)corner | ((uint16_t)error_color << 8);
                } else if (y == 8 || y == 16) {
                    // Horizontal border
                    char hborder = '-';
                    VGA_MEMORY[y * VGA_WIDTH + x] = (uint16_t)hborder | ((uint16_t)error_color << 8);
                } else {
                    // Vertical border
                    char vborder = '|';
                    VGA_MEMORY[y * VGA_WIDTH + x] = (uint16_t)vborder | ((uint16_t)error_color << 8);
                }
            } else {
                // Fill with background
                VGA_MEMORY[y * VGA_WIDTH + x] = (uint16_t)' ' | ((uint16_t)error_color << 8);
            }
        }
    }
    
    // Display error header
    write_string("DRIVER ERROR", error_color, 32, 9);
    
    // Display driver name
    if (driver_id >= 0 && driver_id < (int)(sizeof(driver_names)/sizeof(driver_names[0]))) {
        write_string("Driver:", error_color, 12, 11);
        write_string(driver_names[driver_id], error_color, 20, 11);
    }
    
    // Display error type
    const char* error_str = "Unknown error";
    switch (error_type) {
        case DRIVER_INIT_FAILED: error_str = "Initialization failed"; break;
        case DRIVER_TIMEOUT: error_str = "Timeout"; break;
        case DRIVER_HARDWARE_ERROR: error_str = "Hardware error"; break;
        case DRIVER_MEMORY_ERROR: error_str = "Memory error"; break;
        case DRIVER_IO_ERROR: error_str = "I/O error"; break;
        case DRIVER_NOT_FOUND: error_str = "Device not found"; break;
        case DRIVER_VERSION_MISMATCH: error_str = "Version mismatch"; break;
    }
    
    write_string("Error:", error_color, 12, 12);
    write_string(error_str, error_color, 19, 12);
    
    // Display extra info if provided
    if (extra_info) {
        write_string("Info:", error_color, 12, 13);
        write_string(extra_info, error_color, 18, 13);
    }
    
    // Display recovery instructions
    write_string("Press any key to continue...", error_color, 22, 15);
    
    // Wait for key press (simplified)
    // In real implementation, would wait for keyboard input
    
    // Restore screen after delay
    for (volatile int i = 0; i < 1000000; i++); // Simple delay
    
    // Restore original screen
    for (int i = 0; i < VGA_WIDTH * 25; i++) {
        VGA_MEMORY[i] = saved_screen[i];
    }
    
    error_displayed = false;
}

// Driver initialization wrapper
bool driver_init_safe(int (*init_func)(void), int driver_id, const char* driver_name) {
    int result = init_func();
    if (result != 0) {
        driver_error(DRIVER_INIT_FAILED, driver_id, driver_name);
        return false;
    }
    return true;
}
