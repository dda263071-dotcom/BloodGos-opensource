/**************************************************************
 * Makefile Error Handler - BloodG OS
 * Called when Makefile detects issues
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Function prototypes for kernel functions
void print_string(const char* str);
void terminal_putchar(char c);

// Makefile error types
typedef enum {
    ERROR_FILE_NOT_FOUND,
    ERROR_COMPILATION_FAILED,
    ERROR_LINKING_FAILED,
    ERROR_DISK_CREATION_FAILED,
    ERROR_DEPENDENCY_MISSING,
    ERROR_PERMISSION_DENIED,
    ERROR_OUT_OF_MEMORY
} makefile_error_t;

// Error messages
static const char* error_messages[] = {
    "File not found",
    "Compilation failed",
    "Linking failed",
    "Disk creation failed",
    "Dependency missing",
    "Permission denied",
    "Out of memory"
};

// Show makefile error
void makefile_error(makefile_error_t error_type, const char* filename) {
    // Clear screen with red background
    for (size_t i = 0; i < 80 * 25; i++) {
        terminal_putchar(' ');
        // Set background to red
    }
    
    // Display error header
    print_string("\n\n");
    print_string("╔══════════════════════════════════════════════════════════╗\n");
    print_string("║               MAKEFILE BUILD ERROR                       ║\n");
    print_string("╚══════════════════════════════════════════════════════════╝\n\n");
    
    // Display error type
    print_string("Error Type: ");
    if (error_type < sizeof(error_messages)/sizeof(error_messages[0])) {
        print_string(error_messages[error_type]);
    }
    print_string("\n");
    
    // Display filename if provided
    if (filename) {
        print_string("File: ");
        print_string(filename);
        print_string("\n");
    }
    
    // Display common solutions
    print_string("\nPossible Solutions:\n");
    print_string("1. Check if all required files exist\n");
    print_string("2. Verify compiler and assembler are installed\n");
    print_string("3. Run 'make clean' and try again\n");
    print_string("4. Check file permissions\n");
    print_string("5. Ensure enough disk space is available\n");
    
    // Halt system
    asm volatile ("cli");
    while (1) {
        asm volatile ("hlt");
    }
}

// Check if file exists (simulated)
bool file_exists(const char* filename) {
    // This would check if file exists
    // For now, always return true
    (void)filename;
    return true;
}

// Validate build environment
void validate_build_environment(void) {
    print_string("Validating build environment...\n");
    
    // Check essential files
    const char* essential_files[] = {
        "boot/boot.asm",
        "kernel/kernel.c",
        "Makefile",
        "Linker.ld",
        NULL
    };
    
    for (int i = 0; essential_files[i] != NULL; i++) {
        if (!file_exists(essential_files[i])) {
            makefile_error(ERROR_FILE_NOT_FOUND, essential_files[i]);
        }
    }
    
    print_string("Build environment OK\n");
}
