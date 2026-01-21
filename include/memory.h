/**************************************************************
 * Memory Management Header
 **************************************************************/

#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory allocation
void* simple_malloc(size_t size);
void* kmalloc(size_t size);
void simple_free(void* ptr);
void kfree(void* ptr);

// Specialized buffers
uint8_t* get_sector_buffer(void);
uint8_t* get_dir_buffer(void);
uint8_t* get_fat_buffer(void);
void* allocate_buffer(size_t size, const char* purpose);

// Memory info
struct memory_info {
    uint32_t total;
    uint32_t used;
    uint32_t free;
    uint32_t pool_size;
    uint32_t buffer_count;
};

struct memory_info get_memory_info(void);
void memory_dump_info(void);

// Initialization
void memory_init(void);

// Utilities
void memory_fill_pattern(void* ptr, size_t size, uint8_t pattern);
uint8_t memory_checksum(void* ptr, size_t size);

#endif // MEMORY_H
