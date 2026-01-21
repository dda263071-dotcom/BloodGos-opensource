/**************************************************************
 * Memory Management Header - BloodG OS
 **************************************************************/

#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ==================== MEMORY ALLOCATION ==================== */

/**
 * Simple memory allocator
 * @param size Bytes to allocate
 * @return Pointer to allocated memory or NULL
 */
void* simple_malloc(size_t size);

/**
 * Kernel memory allocator (alias for simple_malloc)
 * @param size Bytes to allocate
 * @return Pointer to allocated memory or NULL
 */
void* kmalloc(size_t size);

/**
 * Free memory (simple implementation - no-op for bump allocator)
 * @param ptr Pointer to free
 */
void simple_free(void* ptr);

/**
 * Kernel memory free (alias for simple_free)
 * @param ptr Pointer to free
 */
void kfree(void* ptr);

/**
 * Allocate aligned memory
 * @param size Bytes to allocate
 * @param alignment Alignment requirement (must be power of 2)
 * @return Pointer to allocated memory or NULL
 */
void* aligned_alloc(size_t size, size_t alignment);

/* ==================== SPECIALIZED BUFFERS ==================== */

/**
 * Get sector buffer (512 bytes)
 * @return Pointer to sector buffer
 */
uint8_t* get_sector_buffer(void);

/**
 * Get directory buffer (7KB for root directory)
 * @return Pointer to directory buffer
 */
uint8_t* get_dir_buffer(void);

/**
 * Get FAT buffer (4.5KB for FAT table)
 * @return Pointer to FAT buffer
 */
uint8_t* get_fat_buffer(void);

/**
 * Allocate buffer with description
 * @param size Bytes to allocate
 * @param purpose Description for debugging
 * @return Pointer to allocated buffer
 */
void* allocate_buffer(size_t size, const char* purpose);

/* ==================== MEMORY INFORMATION ==================== */

/**
 * Memory information structure
 */
struct memory_info {
    uint32_t total;      /**< Total system memory */
    uint32_t used;       /**< Used memory */
    uint32_t free;       /**< Free memory */
    uint32_t pool_size;  /**< Memory pool size */
    uint32_t buffer_count; /**< Number of allocated buffers */
};

/**
 * Get memory information
 * @return Memory information structure
 */
struct memory_info get_memory_info(void);

/**
 * Dump memory information to screen
 */
void memory_dump_info(void);

/* ==================== MEMORY MANAGEMENT ==================== */

/**
 * Initialize memory manager
 */
void memory_init(void);

/**
 * Get memory pool usage
 * @return Bytes used in memory pool
 */
size_t get_memory_usage(void);

/**
 * Get memory pool size
 * @return Total size of memory pool
 */
size_t get_memory_pool_size(void);

/* ==================== MEMORY UTILITIES ==================== */

/**
 * Fill memory with pattern
 * @param ptr Pointer to memory
 * @param size Size in bytes
 * @param pattern Pattern byte
 */
void memory_fill_pattern(void* ptr, size_t size, uint8_t pattern);

/**
 * Calculate checksum of memory block
 * @param ptr Pointer to memory
 * @param size Size in bytes
 * @return Checksum byte
 */
uint8_t memory_checksum(void* ptr, size_t size);

/**
 * Align size up to boundary
 * @param size Size to align
 * @param align Alignment boundary
 * @return Aligned size
 */
size_t align_up(size_t size, size_t align);

#endif // _MEMORY_H
