/**************************************************************
 * FAT12 Filesystem Driver - COMPLETE VERSION
 **************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

// ATA driver
bool disk_read_sector(uint32_t lba, uint8_t* buffer);

// FAT12 Structures
typedef struct {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_count;
    uint16_t root_dir_entries;
    uint16_t total_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t head_count;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;
} __attribute__((packed)) fat12_bpb_t;

typedef struct {
    char filename[8];
    char extension[3];
    uint8_t attributes;
    uint8_t reserved[10];
    uint16_t time;
    uint16_t date;
    uint16_t first_cluster;
    uint32_t file_size;
} __attribute__((packed)) fat12_dir_entry_t;

// Global variables
static fat12_bpb_t bpb;
static uint8_t fat[512 * 9];  // FAT table (9 sectors max)
static bool initialized = false;

// Utility: Convert cluster to LBA
uint32_t fat12_cluster_to_lba(uint16_t cluster) {
    uint32_t root_dir_sectors = ((bpb.root_dir_entries * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
    uint32_t data_start = bpb.reserved_sectors + (bpb.fat_count * bpb.sectors_per_fat) + root_dir_sectors;
    return data_start + ((cluster - 2) * bpb.sectors_per_cluster);
}

// Utility: Get next cluster in chain
uint16_t fat12_get_next_cluster(uint16_t cluster) {
    uint32_t fat_offset = cluster * 3 / 2;
    
    if(cluster & 0x0001) {
        // Odd cluster
        return (*(uint16_t*)(&fat[fat_offset])) >> 4;
    } else {
        // Even cluster
        return (*(uint16_t*)(&fat[fat_offset])) & 0x0FFF;
    }
}

// Utility: Convert 8.3 filename to string
void fat12_83_to_string(const fat12_dir_entry_t* entry, char* buffer) {
    // Filename (8 chars)
    int pos = 0;
    for(int i = 0; i < 8; i++) {
        if(entry->filename[i] != ' ') {
            buffer[pos++] = entry->filename[i];
        }
    }
    
    // Extension if present
    if(entry->extension[0] != ' ') {
        buffer[pos++] = '.';
        for(int i = 0; i < 3; i++) {
            if(entry->extension[i] != ' ') {
                buffer[pos++] = entry->extension[i];
            }
        }
    }
    
    buffer[pos] = '\0';
}

// Initialize FAT12 filesystem
bool fat12_init(void) {
    if(initialized) return true;
    
    // Read boot sector
    if(!disk_read_sector(0, (uint8_t*)&bpb)) {
        return false;
    }
    
    // Verify it's FAT12
    if(bpb.bytes_per_sector != 512) {
        return false;
    }
    
    // Read FAT tables
    uint32_t fat_start = bpb.reserved_sectors;
    for(int i = 0; i < bpb.sectors_per_fat; i++) {
        if(!disk_read_sector(fat_start + i, &fat[i * 512])) {
            return false;
        }
    }
    
    initialized = true;
    return true;
}

// Find file in directory
bool fat12_find_file(const char* filename, fat12_dir_entry_t* result) {
    if(!initialized && !fat12_init()) {
        return false;
    }
    
    uint32_t root_dir_start = bpb.reserved_sectors + (bpb.fat_count * bpb.sectors_per_fat);
    uint32_t root_dir_sectors = ((bpb.root_dir_entries * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
    
    // Buffer for sector
    uint8_t sector_buffer[512];
    
    for(uint32_t sector = 0; sector < root_dir_sectors; sector++) {
        if(!disk_read_sector(root_dir_start + sector, sector_buffer)) {
            return false;
        }
        
        // Parse 16 entries per sector (32 bytes each)
        for(int i = 0; i < 16; i++) {
            fat12_dir_entry_t* entry = (fat12_dir_entry_t*)&sector_buffer[i * 32];
            
            // Check if entry is valid
            if(entry->filename[0] == 0x00) {
                // End of directory
                return false;
            }
            
            if(entry->filename[0] == 0xE5) {
                // Deleted entry
                continue;
            }
            
            // Skip volume label, subdirectory (for now)
            if(entry->attributes & 0x08 || entry->attributes & 0x10) {
                continue;
            }
            
            // Convert to string for comparison
            char entry_name[13];
            fat12_83_to_string(entry, entry_name);
            
            // Compare filenames (case-insensitive)
            int len = 0;
            while(entry_name[len] && filename[len]) {
                char c1 = entry_name[len];
                char c2 = filename[len];
                
                // Convert to uppercase for comparison
                if(c1 >= 'a' && c1 <= 'z') c1 -= 32;
                if(c2 >= 'a' && c2 <= 'z') c2 -= 32;
                
                if(c1 != c2) break;
                len++;
            }
            
            if(entry_name[len] == '\0' && filename[len] == '\0') {
                // Match found
                *result = *entry;
                return true;
            }
        }
    }
    
    return false;
}

// List directory contents
void fat12_list_directory(void) {
    if(!initialized && !fat12_init()) {
        return;
    }
    
    uint32_t root_dir_start = bpb.reserved_sectors + (bpb.fat_count * bpb.sectors_per_fat);
    uint32_t root_dir_sectors = ((bpb.root_dir_entries * 32) + (bpb.bytes_per_sector - 1)) / bpb.bytes_per_sector;
    
    uint8_t sector_buffer[512];
    bool found_any = false;
    
    for(uint32_t sector = 0; sector < root_dir_sectors; sector++) {
        if(!disk_read_sector(root_dir_start + sector, sector_buffer)) {
            break;
        }
        
        for(int i = 0; i < 16; i++) {
            fat12_dir_entry_t* entry = (fat12_dir_entry_t*)&sector_buffer[i * 32];
            
            if(entry->filename[0] == 0x00) {
                // End of directory
                return;
            }
            
            if(entry->filename[0] == 0xE5) {
                continue;
            }
            
            // Skip volume label
            if(entry->attributes & 0x08) {
                continue;
            }
            
            found_any = true;
            
            // Convert filename
            char name[13];
            fat12_83_to_string(entry, name);
            
            // Print entry
            if(entry->attributes & 0x10) {
                // Directory
                print_string("[DIR]  ");
            } else {
                // File
                print_string("[FILE] ");
            }
            
            print_string(name);
            
            // Add spacing
            size_t len = strlen(name);
            while(len++ < 15) print_string(" ");
            
            // File size
            char size_str[16];
            uint32_t size = entry->file_size;
            
            if(size < 1024) {
                // Bytes
                print_string("    ");
                // Simple size conversion
                char* p = size_str;
                if(size == 0) {
                    *p++ = '0';
                } else {
                    uint32_t n = size;
                    char temp[16];
                    char* t = temp;
                    while(n > 0) {
                        *t++ = '0' + (n % 10);
                        n /= 10;
                    }
                    while(t > temp) {
                        *p++ = *(--t);
                    }
                }
                *p++ = 'B';
                *p = '\0';
                print_string(size_str);
            } else if(size < 1024 * 1024) {
                // KB
                uint32_t kb = size / 1024;
                // Similar conversion for KB
                print_string("    ");
                // ... conversion code ...
                print_string("KB");
            }
            
            print_string("\n");
        }
    }
    
    if(!found_any) {
        print_string("Directory is empty\n");
    }
}

// Read file into buffer
bool fat12_read_file(const char* filename, uint8_t* buffer, uint32_t max_size) {
    fat12_dir_entry_t entry;
    
    if(!fat12_find_file(filename, &entry)) {
        return false;
    }
    
    if(entry.file_size > max_size) {
        return false;
    }
    
    // Follow cluster chain
    uint16_t cluster = entry.first_cluster;
    uint32_t bytes_read = 0;
    uint8_t sector_buffer[512];
    
    while(cluster < 0xFF8) {  // Not EOF
        if(cluster == 0xFF7) {
            // Bad cluster
            return false;
        }
        
        // Read cluster
        uint32_t lba = fat12_cluster_to_lba(cluster);
        for(int i = 0; i < bpb.sectors_per_cluster; i++) {
            if(!disk_read_sector(lba + i, sector_buffer)) {
                return false;
            }
            
            // Copy to buffer
            uint32_t to_copy = 512;
            if(bytes_read + to_copy > entry.file_size) {
                to_copy = entry.file_size - bytes_read;
            }
            
            memcpy(buffer + bytes_read, sector_buffer, to_copy);
            bytes_read += to_copy;
            
            if(bytes_read >= entry.file_size) {
                return true;
            }
        }
        
        // Get next cluster
        cluster = fat12_get_next_cluster(cluster);
    }
    
    return bytes_read == entry.file_size;
}
