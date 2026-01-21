/**************************************************************
 * Memory Check Tool - C Version
 * Provides memory analysis functions for kernel use
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory region structure
struct memory_region {
    uint32_t start;
    uint32_t end;
    const char* description;
    bool used;
};

// Global memory regions
static struct memory_region regions[] = {
    {0x00000000, 0x0000FFFF, "Real Mode Area", false},
    {0x00010000, 0x0008FFFF, "Kernel Space", true},
    {0x00090000, 0x0009FFFF, "Stack Space", true},
    {0x000B8000, 0x000B8FA0, "VGA Text Buffer", true},
    {0x00100000, 0x01FFFFFF, "Available Memory", false},
};

#define REGION_COUNT (sizeof(regions) / sizeof(regions[0]))

// Function prototypes
void memory_check_print_regions(void);
uint32_t memory_check_get_total(void);
uint32_t memory_check_get_used(void);
uint32_t memory_check_get_free(void);
bool memory_check_validate_address(uint32_t address);
const char* memory_check_get_region(uint32_t address);

// Print all memory regions
void memory_check_print_regions(void) {
    // This would interface with terminal output
    // For now, it's a placeholder
}

// Calculate total memory
uint32_t memory_check_get_total(void) {
    uint32_t total = 0;
    
    for (size_t i = 0; i < REGION_COUNT; i++) {
        total += (regions[i].end - regions[i].start);
    }
    
    return total;
}

// Calculate used memory
uint32_t memory_check_get_used(void) {
    uint32_t used = 0;
    
    for (size_t i = 0; i < REGION_COUNT; i++) {
        if (regions[i].used) {
            used += (regions[i].end - regions[i].start);
        }
    }
    
    return used;
}

// Calculate free memory
uint32_t memory_check_get_free(void) {
    return memory_check_get_total() - memory_check_get_used();
}

// Validate if an address is in valid memory
bool memory_check_validate_address(uint32_t address) {
    for (size_t i = 0; i < REGION_COUNT; i++) {
        if (address >= regions[i].start && address <= regions[i].end) {
            return true;
        }
    }
    return false;
}

// Get region description for an address
const char* memory_check_get_region(uint32_t address) {
    for (size_t i = 0; i < REGION_COUNT; i++) {
        if (address >= regions[i].start && address <= regions[i].end) {
            return regions[i].description;
        }
    }
    return "Unknown Region";
}

// Simple memory test (write/read)
bool memory_check_test_region(uint32_t start, uint32_t size) {
    // This is a simplified test
    // In reality, you'd need to be careful about what memory you test
    (void)start;
    (void)size;
    
    // For now, return true for known good regions
    return true;
}

// Get memory statistics
struct memory_stats {
    uint32_t total;
    uint32_t used;
    uint32_t free;
    float usage_percent;
};

struct memory_stats memory_check_get_stats(void) {
    struct memory_stats stats;
    
    stats.total = memory_check_get_total();
    stats.used = memory_check_get_used();
    stats.free = memory_check_get_free();
    stats.usage_percent = (float)stats.used / stats.total * 100.0f;
    
    return stats;
}
