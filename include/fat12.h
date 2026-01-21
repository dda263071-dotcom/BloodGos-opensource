/**************************************************************
 * FAT12 Filesystem Header - BloodG OS
 **************************************************************/

#ifndef _FAT12_H
#define _FAT12_H

#include <stdint.h>
#include <stdbool.h>

/* ==================== FAT12 STRUCTURES ==================== */

#pragma pack(push, 1)

/**
 * FAT12 BIOS Parameter Block
 */
typedef struct {
    uint8_t  jump[3];           /**< Jump instruction */
    char     oem[8];            /**< OEM name */
    uint16_t bytes_per_sector;  /**< Bytes per sector (512) */
    uint8_t  sectors_per_cluster; /**< Sectors per cluster (1) */
    uint16_t reserved_sectors;  /**< Reserved sectors (1) */
    uint8_t  fat_count;         /**< Number of FATs (2) */
    uint16_t root_dir_entries;  /**< Root directory entries (224) */
    uint16_t total_sectors;     /**< Total sectors (2880 for 1.44MB) */
    uint8_t  media_type;        /**< Media descriptor (0xF0) */
    uint16_t sectors_per_fat;   /**< Sectors per FAT (9) */
    uint16_t sectors_per_track; /**< Sectors per track (18) */
    uint16_t head_count;        /**< Number of heads (2) */
    uint32_t hidden_sectors;    /**< Hidden sectors */
    uint32_t large_sector_count; /**< Large sector count */
} fat12_bpb_t;

/**
 * FAT12 Directory Entry
 */
typedef struct {
    char     filename[8];       /**< File name (8 chars) */
    char     extension[3];      /**< File extension (3 chars) */
    uint8_t  attributes;        /**< File attributes */
    uint8_t  reserved[10];      /**< Reserved */
    uint16_t time;              /**< Last modified time */
    uint16_t date;              /**< Last modified date */
    uint16_t first_cluster;     /**< First cluster number */
    uint32_t file_size;         /**< File size in bytes */
} fat12_dir_entry_t;

#pragma pack(pop)

/* ==================== FILE ATTRIBUTES ==================== */

#define FAT12_ATTR_READ_ONLY  0x01
#define FAT12_ATTR_HIDDEN     0x02
#define FAT12_ATTR_SYSTEM     0x04
#define FAT12_ATTR_VOLUME_ID  0x08
#define FAT12_ATTR_DIRECTORY  0x10
#define FAT12_ATTR_ARCHIVE    0x20

/* ==================== FAT12 FUNCTIONS ==================== */

/**
 * Initialize FAT12 filesystem
 * @return true if successful, false otherwise
 */
bool fat12_init(void);

/**
 * List directory contents
 */
void fat12_list_directory(void);

/**
 * Read file into buffer
 * @param filename File to read
 * @param buffer Destination buffer
 * @param max_size Maximum buffer size
 * @return true if successful, false otherwise
 */
bool fat12_read_file(const char* filename, uint8_t* buffer, uint32_t max_size);

/**
 * Find file in directory
 * @param filename File to find
 * @param result Pointer to store directory entry
 * @return true if found, false otherwise
 */
bool fat12_find_file(const char* filename, fat12_dir_entry_t* result);

/**
 * Convert cluster number to LBA sector
 * @param cluster Cluster number
 * @return LBA sector number
 */
uint32_t fat12_cluster_to_lba(uint16_t cluster);

/**
 * Get next cluster in chain
 * @param cluster Current cluster
 * @return Next cluster number
 */
uint16_t fat12_get_next_cluster(uint16_t cluster);

/**
 * Convert 8.3 filename to string
 * @param entry Directory entry
 * @param buffer Output buffer (must be at least 13 bytes)
 */
void fat12_83_to_string(const fat12_dir_entry_t* entry, char* buffer);

/**
 * Get file information
 * @param filename File name
 * @param info Pointer to store directory entry
 * @return true if successful, false otherwise
 */
bool fat12_get_file_info(const char* filename, fat12_dir_entry_t* info);

/**
 * Check if filesystem is mounted
 * @return true if mounted, false otherwise
 */
bool fat12_is_mounted(void);

/**
 * Get free space in bytes
 * @return Free space in bytes
 */
uint32_t fat12_get_free_space(void);

/**
 * Get total space in bytes
 * @return Total space in bytes
 */
uint32_t fat12_get_total_space(void);

/**
 * Get filesystem information
 * @param total_clusters Output: total clusters
 * @param free_clusters Output: free clusters
 * @param used_clusters Output: used clusters
 * @return true if successful, false otherwise
 */
bool fat12_get_fs_info(uint32_t* total_clusters, uint32_t* free_clusters, uint32_t* used_clusters);

#endif // _FAT12_H
