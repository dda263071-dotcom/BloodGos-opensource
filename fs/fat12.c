/**************************************************************
 * FAT12 Filesystem Driver - COMPLETE VERSION
 * Using proper headers and memory management
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Include our custom headers
#include "string.h"
#include "io.h"
#include "memory.h"
#include "fat12.h"

// External function declarations (from kernel/ata.c)
bool disk_read_sector(uint32_t lba, uint8_t* buffer);
bool disk_write_sector(uint32_t lba, uint8_t* buffer);

// Filesystem state
static fat12_bpb_t bpb;
static bool initialized = false;
static uint8_t* fat_cache = NULL;  // FAT cache buffer
static uint8_t* root_dir_cache = NULL;  // Root directory cache

// Local helper function prototypes
static uint32_t calculate_root_dir_sectors(void);
static uint32_t calculate_data_start_sector(void);
static bool read_fat_table(void);
static bool read_root_directory(void);
static bool validate_fat12(void);

// Initialize FAT12 filesystem
bool fat12_init(void) {
    if (initialized) {
        return true;
    }
    
    print_string("Initializing FAT12 filesystem...\n");
    
    // Allocate buffers
    fat_cache = get_fat_buffer();
    root_dir_cache = get_dir_buffer();
    
    if (!fat_cache || !root_dir_cache) {
        print_string("Error: Cannot allocate filesystem buffers\n");
        return false;
    }
    
    // Read boot sector
    if (!disk_read_sector(0, (uint8_t*)&bpb)) {
        print_string("Error: Cannot read boot sector\n");
        return false;
    }
    
    // Validate FAT12
    if (!validate_fat12()) {
        print_string("Error: Invalid FAT12 filesystem\n");
        return false;
    }
    
    // Read FAT table
    if (!read_fat_table()) {
        print_string("Error: Cannot read FAT table\n");
        return false;
    }
    
    // Read root directory
    if (!read_root_directory()) {
        print_string("Error: Cannot read root directory\n");
        return false;
    }
    
    initialized = true;
    
    // Print filesystem info
    print_string("FAT12 Filesystem mounted:\n");
    
    char buffer[32];
    
    // Total sectors
    itoa(bpb.total_sectors, buffer, 10);
    print_string("  Total sectors: ");
    print_string(buffer);
    print_string("\n");
    
    // Bytes per sector
    itoa(bpb.bytes_per_sector, buffer, 10);
    print_string("  Bytes per sector: ");
    print_string(buffer);
    print_string("\n");
    
    // Root entries
    itoa(bpb.root_dir_entries, buffer, 10);
    print_string("  Root entries: ");
    print_string(buffer);
    print_string("\n");
    
    return true;
}

// Validate FAT12 filesystem
static bool validate_fat12(void) {
    // Check signature
    if (bpb.bytes_per_sector != 512) {
        return false;
    }
    
    if (bpb.sectors_per_cluster == 0) {
        return false;
    }
    
    if (bpb.fat_count == 0) {
        return false;
    }
    
    // Check for valid FAT12 (total sectors <= 4084 clusters)
    uint32_t data_sectors = bpb.total_sectors - 
                           (bpb.reserved_sectors + 
                            (bpb.fat_count * bpb.sectors_per_fat) + 
                            calculate_root_dir_sectors());
    
    uint32_t total_clusters = data_sectors / bpb.sectors_per_cluster;
    
    if (total_clusters > 4084) {  // FAT12 max clusters
        return false;
    }
    
    return true;
}

// Read FAT table into cache
static bool read_fat_table(void) {
    uint32_t fat_start = bpb.reserved_sectors;
    
    for (uint16_t i = 0; i < bpb.sectors_per_fat; i++) {
        if (!disk_read_sector(fat_start + i, &fat_cache[i * 512])) {
            return false;
        }
    }
    
    return true;
}

// Read root directory into cache
static bool read_root_directory(void) {
    uint32_t root_dir_start = bpb.reserved_sectors + (bpb.fat_count * bpb.sectors_per_fat);
    uint32_t root_dir_sectors = calculate_root_dir_sectors();
    
    for (uint32_t i = 0; i < root_dir_sectors; i++) {
        if (!disk_read_sector(root_dir_start + i, &root_dir_cache[i * 512])) {
            return false;
        }
    }
    
    return true;
}

// Calculate root directory size in sectors
static uint32_t calculate_root_dir_sectors(void) {
    return ((bpb.root_dir_entries * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
}

// Calculate data area start sector
static uint32_t calculate_data_start_sector(void) {
    return bpb.reserved_sectors + 
           (bpb.fat_count * bpb.sectors_per_fat) + 
           calculate_root_dir_sectors();
}

// Convert cluster number to LBA
uint32_t fat12_cluster_to_lba(uint16_t cluster) {
    if (cluster < 2) {
        return 0;  // Invalid cluster
    }
    
    uint32_t data_start = calculate_data_start_sector();
    return data_start + ((cluster - 2) * bpb.sectors_per_cluster);
}

// Get next cluster in chain
uint16_t fat12_get_next_cluster(uint16_t cluster) {
    if (cluster < 2 || cluster >= 4085) {  // FAT12 max cluster
        return 0xFFF;  // Invalid
    }
    
    uint32_t fat_offset = cluster * 3 / 2;
    
    if (fat_offset >= (bpb.sectors_per_fat * 512)) {
        return 0xFFF;  // Out of bounds
    }
    
    if (cluster & 0x0001) {
        // Odd cluster: high 12 bits
        return (fat_cache[fat_offset] >> 4) | (fat_cache[fat_offset + 1] << 4);
    } else {
        // Even cluster: low 12 bits
        return (fat_cache[fat_offset] | (fat_cache[fat_offset + 1] << 8)) & 0x0FFF;
    }
}

// Convert 8.3 filename to string
void fat12_83_to_string(const fat12_dir_entry_t* entry, char* buffer) {
    // Copy filename (8 chars)
    int pos = 0;
    for (int i = 0; i < 8; i++) {
        if (entry->filename[i] != ' ') {
            buffer[pos++] = entry->filename[i];
        }
    }
    
    // Add extension if present
    if (entry->extension[0] != ' ') {
        buffer[pos++] = '.';
        for (int i = 0; i < 3; i++) {
            if (entry->extension[i] != ' ') {
                buffer[pos++] = entry->extension[i];
            }
        }
    }
    
    buffer[pos] = '\0';
}

// Find file in directory
bool fat12_find_file(const char* filename, fat12_dir_entry_t* result) {
    if (!initialized) {
        return false;
    }
    
    // Convert filename to uppercase for comparison
    char search_name[13];
    char upper_filename[13];
    
    // Copy and convert to uppercase
    strcpy(upper_filename, filename);
    for (int i = 0; upper_filename[i]; i++) {
        if (upper_filename[i] >= 'a' && upper_filename[i] <= 'z') {
            upper_filename[i] = upper_filename[i] - 'a' + 'A';
        }
    }
    
    // Search through root directory cache
    for (uint16_t i = 0; i < bpb.root_dir_entries; i++) {
        fat12_dir_entry_t* entry = (fat12_dir_entry_t*)&root_dir_cache[i * 32];
        
        // Check if entry is valid
        if (entry->filename[0] == 0x00) {
            // End of directory
            break;
        }
        
        if (entry->filename[0] == 0xE5) {
            // Deleted entry
            continue;
        }
        
        // Skip volume label and directories for now
        if (entry->attributes & 0x08 || entry->attributes & 0x10) {
            continue;
        }
        
        // Convert entry name
        fat12_83_to_string(entry, search_name);
        
        // Convert to uppercase for comparison
        for (int j = 0; search_name[j]; j++) {
            if (search_name[j] >= 'a' && search_name[j] <= 'z') {
                search_name[j] = search_name[j] - 'a' + 'A';
            }
        }
        
        // Compare
        if (strcmp(search_name, upper_filename) == 0) {
            *result = *entry;
            return true;
        }
    }
    
    return false;
}

// Get file information
bool fat12_get_file_info(const char* filename, fat12_dir_entry_t* info) {
    return fat12_find_file(filename, info);
}

// List directory contents
void fat12_list_directory(void) {
    if (!initialized) {
        print_string("Filesystem not initialized\n");
        return;
    }
    
    print_string("\nDirectory listing:\n");
    print_string("==================\n");
    
    bool found_any = false;
    char filename[13];
    
    for (uint16_t i = 0; i < bpb.root_dir_entries; i++) {
        fat12_dir_entry_t* entry = (fat12_dir_entry_t*)&root_dir_cache[i * 32];
        
        // Check if entry is valid
        if (entry->filename[0] == 0x00) {
            // End of directory
            break;
        }
        
        if (entry->filename[0] == 0xE5) {
            // Deleted entry
            continue;
        }
        
        // Skip volume label
        if (entry->attributes & 0x08) {
            continue;
        }
        
        found_any = true;
        
        // Convert filename
        fat12_83_to_string(entry, filename);
        
        // Print entry type
        if (entry->attributes & 0x10) {
            print_string("[DIR]  ");
        } else {
            print_string("[FILE] ");
        }
        
        // Print filename
        print_string(filename);
        
        // Align columns
        size_t name_len = strlen(filename);
        while (name_len++ < 15) {
            print_string(" ");
        }
        
        // Print file size if it's a file
        if (!(entry->attributes & 0x10)) {
            char size_str[16];
            
            if (entry->file_size < 1024) {
                itoa(entry->file_size, size_str, 10);
                print_string(size_str);
                print_string(" bytes");
            } else if (entry->file_size < 1024 * 1024) {
                itoa(entry->file_size / 1024, size_str, 10);
                print_string(size_str);
                print_string(" KB");
            } else {
                itoa(entry->file_size / (1024 * 1024), size_str, 10);
                print_string(size_str);
                print_string(" MB");
            }
        }
        
        print_string("\n");
    }
    
    if (!found_any) {
        print_string("(empty directory)\n");
    }
    
    print_string("\n");
}

// Read file into buffer
bool fat12_read_file(const char* filename, uint8_t* buffer, uint32_t max_size) {
    if (!initialized) {
        return false;
    }
    
    // Find the file
    fat12_dir_entry_t entry;
    if (!fat12_find_file(filename, &entry)) {
        return false;
    }
    
    // Check buffer size
    if (entry.file_size > max_size) {
        print_string("Error: Buffer too small for file\n");
        return false;
    }
    
    // Follow cluster chain
    uint16_t cluster = entry.first_cluster;
    uint32_t bytes_read = 0;
    uint8_t sector_buffer[512];
    
    while (cluster < 0xFF8) {  // Not EOF
        if (cluster == 0xFF7) {
            // Bad cluster
            print_string("Error: Bad cluster encountered\n");
            return false;
        }
        
        // Read cluster
        uint32_t lba = fat12_cluster_to_lba(cluster);
        for (int i = 0; i < bpb.sectors_per_cluster; i++) {
            if (!disk_read_sector(lba + i, sector_buffer)) {
                print_string("Error: Cannot read sector\n");
                return false;
            }
            
            // Calculate bytes to copy
            uint32_t to_copy = 512;
            if (bytes_read + to_copy > entry.file_size) {
                to_copy = entry.file_size - bytes_read;
            }
            
            // Copy to buffer
            memcpy(buffer + bytes_read, sector_buffer, to_copy);
            bytes_read += to_copy;
            
            // Check if done
            if (bytes_read >= entry.file_size) {
                return true;
            }
        }
        
        // Get next cluster
        cluster = fat12_get_next_cluster(cluster);
    }
    
    return bytes_read == entry.file_size;
}

// Check if filesystem is mounted
bool fat12_is_mounted(void) {
    return initialized;
}

// Get free space (simplified - counts free clusters)
uint32_t fat12_get_free_space(void) {
    if (!initialized) {
        return 0;
    }
    
    uint32_t free_clusters = 0;
    
    // Count free clusters in FAT (cluster 2 and up)
    for (uint16_t cluster = 2; cluster < 4085; cluster++) {
        uint16_t fat_entry = fat12_get_next_cluster(cluster);
        if (fat_entry == 0x000) {  // Free cluster
            free_clusters++;
        }
    }
    
    return free_clusters * bpb.sectors_per_cluster * bpb.bytes_per_sector;
}

// Get total space
uint32_t fat12_get_total_space(void) {
    if (!initialized) {
        return 0;
    }
    
    uint32_t data_sectors = bpb.total_sectors - 
                           (bpb.reserved_sectors + 
                            (bpb.fat_count * bpb.sectors_per_fat) + 
                            calculate_root_dir_sectors());
    
    return data_sectors * bpb.bytes_per_sector;
}
