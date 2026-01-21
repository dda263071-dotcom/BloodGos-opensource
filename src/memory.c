/**************************************************************
 * Memory Management for BloodG OS
 * Enhanced for filesystem support
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>  // Menggunakan memcpy/memset dari string.c

// Memory Manager Configuration
#define MEMORY_POOL_SIZE (1 * 1024 * 1024)  // 1MB untuk kernel + filesystem
#define ALIGNMENT 16  // Align memory to 16 bytes for performance

// Memory block header (untuk implementasi malloc/free di masa depan)
struct mem_block {
    size_t size;
    bool free;
    struct mem_block* next;
};

// Simple memory manager dengan peningkatan
static uint8_t memory_pool[MEMORY_POOL_SIZE] __attribute__((aligned(ALIGNMENT)));
static size_t memory_offset = 0;
static struct mem_block* block_list = NULL;

// Align size ke batas tertentu
static size_t align_up(size_t size, size_t align) {
    return (size + align - 1) & ~(align - 1);
}

// Simple bump allocator (untuk sekarang)
void* simple_malloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size
    size = align_up(size, ALIGNMENT);
    
    // Check jika cukup memory
    if (memory_offset + size > MEMORY_POOL_SIZE) {
        return NULL;  // Out of memory
    }
    
    // Allocate
    void* ptr = &memory_pool[memory_offset];
    memory_offset += size;
    
    return ptr;
}

// Untuk kompatibilitas dengan kode yang sudah ada
void* kmalloc(size_t size) {
    return simple_malloc(size);
}

// Free masih sederhana (nanti bisa diimplementasi free list)
void simple_free(void* ptr) {
    // Untuk bump allocator, free tidak melakukan apa-apa
    // Nanti bisa di-upgrade ke proper allocator
    (void)ptr;
}

void kfree(void* ptr) {
    simple_free(ptr);
}

// Allocate memory untuk filesystem buffers
void* allocate_buffer(size_t size, const char* purpose) {
    void* buffer = simple_malloc(size);
    
    // Debug info (nanti bisa di-enable dengan flag)
    (void)purpose;
    // print_string("Allocated ");
    // print_string(purpose);
    // print_string(" buffer: ");
    // char size_str[16];
    // itoa(size, size_str, 10);
    // print_string(size_str);
    // print_string(" bytes\n");
    
    return buffer;
}

// Get filesystem buffer (512 bytes untuk sector)
uint8_t* get_sector_buffer(void) {
    static uint8_t* sector_buffer = NULL;
    
    if (!sector_buffer) {
        sector_buffer = (uint8_t*)allocate_buffer(512, "sector buffer");
    }
    
    return sector_buffer;
}

// Get filesystem directory buffer
uint8_t* get_dir_buffer(void) {
    static uint8_t* dir_buffer = NULL;
    
    if (!dir_buffer) {
        dir_buffer = (uint8_t*)allocate_buffer(512 * 14, "directory buffer");
    }
    
    return dir_buffer;
}

// Get filesystem FAT buffer
uint8_t* get_fat_buffer(void) {
    static uint8_t* fat_buffer = NULL;
    
    if (!fat_buffer) {
        fat_buffer = (uint8_t*)allocate_buffer(512 * 9, "FAT buffer");
    }
    
    return fat_buffer;
}

// Memory utilities yang TIDAK duplikat dengan string.c
// Di sini kita hanya menyediakan fungsi yang spesifik untuk memory management

// Clear memory region dengan pattern (untuk debugging)
void memory_fill_pattern(void* ptr, size_t size, uint8_t pattern) {
    memset(ptr, pattern, size);
}

// Check memory integrity (simple checksum)
uint8_t memory_checksum(void* ptr, size_t size) {
    uint8_t* p = (uint8_t*)ptr;
    uint8_t checksum = 0;
    
    for (size_t i = 0; i < size; i++) {
        checksum ^= p[i];
    }
    
    return checksum;
}

// Memory detection - lebih akurat
struct memory_info {
    uint32_t total;
    uint32_t used;
    uint32_t free;
    uint32_t pool_size;
    uint32_t buffer_count;
};

struct memory_info get_memory_info(void) {
    struct memory_info info;
    
    // Informasi memory (simulasi - nanti bisa diambil dari BIOS)
    info.total = 64 * 1024 * 1024;  // 64MB total
    info.used = memory_offset + 
                (64 * 1024) +       // Kernel + stack
                (512 * 14) +        // Directory buffer
                (512 * 9) +         // FAT buffer
                (512);              // Sector buffer
    
    info.free = info.total - info.used;
    info.pool_size = MEMORY_POOL_SIZE;
    info.buffer_count = 3;  // sector, dir, FAT buffers
    
    return info;
}

// Dump memory allocation (untuk debugging)
void memory_dump_info(void) {
    struct memory_info info = get_memory_info();
    
    print_string("\nMemory Information:\n");
    print_string("===================\n");
    
    // Total system memory
    print_string("System Memory:\n");
    print_string("  Total: ");
    // Convert to string dan print
    // Implementasi sederhana:
    char buf[32];
    
    // Total
    uint32_t total_mb = info.total / (1024 * 1024);
    print_string("    ");
    // ... print total_mb
    print_string(" MB\n");
    
    // Pool info
    print_string("Memory Pool:\n");
    print_string("  Size: ");
    uint32_t pool_kb = MEMORY_POOL_SIZE / 1024;
    // ... print pool_kb
    print_string(" KB\n");
    
    print_string("  Used: ");
    // ... print memory_offset
    print_string(" bytes\n");
    
    print_string("Buffers allocated:\n");
    print_string("  Sector buffer: 512 bytes\n");
    print_string("  Directory buffer: ");
    // ... print 512*14
    print_string(" bytes\n");
    print_string("  FAT buffer: ");
    // ... print 512*9
    print_string(" bytes\n");
}

// Initialize memory manager
void memory_init(void) {
    // Initialize memory pool
    memory_offset = 0;
    
    // Clear the entire pool
    memset(memory_pool, 0, MEMORY_POOL_SIZE);
    
    // Pre-allocate essential buffers
    get_sector_buffer();
    get_dir_buffer();
    get_fat_buffer();
    
    print_string("Memory manager initialized: ");
    char size_str[16];
    // itoa(MEMORY_POOL_SIZE/1024, size_str, 10);
    print_string(size_str);
    print_string("KB pool\n");
}
