/**************************************************************
 * ATA (IDE) Driver Header - BloodG OS
 **************************************************************/

#ifndef _ATA_H
#define _ATA_H

#include <stdint.h>
#include <stdbool.h>

/* ==================== ATA REGISTERS ==================== */

// Primary ATA Controller
#define ATA_PRIMARY_DATA        0x1F0
#define ATA_PRIMARY_ERROR       0x1F1
#define ATA_PRIMARY_FEATURES    0x1F1
#define ATA_PRIMARY_SECTOR_CNT  0x1F2
#define ATA_PRIMARY_LBA_LOW     0x1F3
#define ATA_PRIMARY_LBA_MID     0x1F4
#define ATA_PRIMARY_LBA_HIGH    0x1F5
#define ATA_PRIMARY_DRIVE_SEL   0x1F6
#define ATA_PRIMARY_COMMAND     0x1F7
#define ATA_PRIMARY_STATUS      0x1F7
#define ATA_PRIMARY_ALT_STATUS  0x3F6

// Secondary ATA Controller
#define ATA_SECONDARY_DATA      0x170
#define ATA_SECONDARY_ERROR     0x171
#define ATA_SECONDARY_FEATURES  0x171
#define ATA_SECONDARY_SECTOR_CNT 0x172
#define ATA_SECONDARY_LBA_LOW   0x173
#define ATA_SECONDARY_LBA_MID   0x174
#define ATA_SECONDARY_LBA_HIGH  0x175
#define ATA_SECONDARY_DRIVE_SEL 0x176
#define ATA_SECONDARY_COMMAND   0x177
#define ATA_SECONDARY_STATUS    0x177
#define ATA_SECONDARY_ALT_STATUS 0x376

/* ==================== ATA STATUS BITS ==================== */

#define ATA_SR_BSY      0x80    /**< Busy */
#define ATA_SR_DRDY     0x40    /**< Drive ready */
#define ATA_SR_DF       0x20    /**< Drive write fault */
#define ATA_SR_DSC      0x10    /**< Drive seek complete */
#define ATA_SR_DRQ      0x08    /**< Data request ready */
#define ATA_SR_CORR     0x04    /**< Corrected data */
#define ATA_SR_IDX      0x02    /**< Index */
#define ATA_SR_ERR      0x01    /**< Error */

/* ==================== ATA ERROR BITS ==================== */

#define ATA_ER_BBK      0x80    /**< Bad block */
#define ATA_ER_UNC      0x40    /**< Uncorrectable data */
#define ATA_ER_MC       0x20    /**< Media changed */
#define ATA_ER_IDNF     0x10    /**< ID mark not found */
#define ATA_ER_MCR      0x08    /**< Media change request */
#define ATA_ER_ABRT     0x04    /**< Command aborted */
#define ATA_ER_TK0NF    0x02    /**< Track 0 not found */
#define ATA_ER_AMNF     0x01    /**< No address mark */

/* ==================== ATA COMMANDS ==================== */

#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_IDENTIFY        0xEC

/* ==================== ATA DRIVE SELECTION ==================== */

#define ATA_DRIVE_MASTER        0xA0
#define ATA_DRIVE_SLAVE         0xB0
#define ATA_LBA_MODE            0x40

/* ==================== ATA FUNCTIONS ==================== */

/**
 * Initialize ATA controller
 * @return true if successful, false otherwise
 */
bool ata_init(void);

/**
 * Read sector from disk (LBA addressing)
 * @param lba Logical Block Address
 * @param buffer Destination buffer (must be at least 512 bytes)
 * @return true if successful, false otherwise
 */
bool disk_read_sector(uint32_t lba, uint8_t* buffer);

/**
 * Write sector to disk (LBA addressing)
 * @param lba Logical Block Address
 * @param buffer Source buffer (must be at least 512 bytes)
 * @return true if successful, false otherwise
 */
bool disk_write_sector(uint32_t lba, uint8_t* buffer);

/**
 * Read multiple sectors
 * @param lba Starting LBA
 * @param count Number of sectors to read
 * @param buffer Destination buffer
 * @return true if successful, false otherwise
 */
bool disk_read_sectors(uint32_t lba, uint8_t count, uint8_t* buffer);

/**
 * Write multiple sectors
 * @param lba Starting LBA
 * @param count Number of sectors to write
 * @param buffer Source buffer
 * @return true if successful, false otherwise
 */
bool disk_write_sectors(uint32_t lba, uint8_t count, uint8_t* buffer);

/**
 * Identify ATA device
 * @param buffer Buffer for identify data (512 bytes)
 * @return true if successful, false otherwise
 */
bool ata_identify(uint16_t* buffer);

/**
 * Check if drive is ready
 * @return true if ready, false otherwise
 */
bool ata_drive_ready(void);

/**
 * Wait for drive to be ready
 * @return true if ready, false on timeout
 */
bool ata_wait_ready(void);

/**
 * Wait for data request
 * @return true if ready, false on timeout
 */
bool ata_wait_data_request(void);

/**
 * Flush drive cache
 * @return true if successful, false otherwise
 */
bool ata_flush_cache(void);

/**
 * Get drive information
 * @param model Output: model string (41 characters max)
 * @param serial Output: serial string (20 characters max)
 * @param sectors Output: total sectors
 * @return true if successful, false otherwise
 */
bool ata_get_drive_info(char* model, char* serial, uint32_t* sectors);

#endif // _ATA_H
