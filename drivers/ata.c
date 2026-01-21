/**************************************************************
 * ATA (IDE) Disk Driver - BloodG OS
 * Complete PIO implementation with error handling
 **************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "io.h"
#include "string.h"

// ATA Registers for Primary Controller
#define ATA_DATA        0x1F0
#define ATA_ERROR       0x1F1
#define ATA_FEATURES    0x1F1
#define ATA_SECTOR_CNT  0x1F2
#define ATA_LBA_LOW     0x1F3
#define ATA_LBA_MID     0x1F4
#define ATA_LBA_HIGH    0x1F5
#define ATA_DRIVE_SEL   0x1F6
#define ATA_COMMAND     0x1F7
#define ATA_STATUS      0x1F7
#define ATA_ALT_STATUS  0x3F6

// Status bits
#define ATA_SR_BSY      0x80    // Busy
#define ATA_SR_DRDY     0x40    // Drive ready
#define ATA_SR_DF       0x20    // Drive write fault
#define ATA_SR_DSC      0x10    // Drive seek complete
#define ATA_SR_DRQ      0x08    // Data request ready
#define ATA_SR_CORR     0x04    // Corrected data
#define ATA_SR_IDX      0x02    // Index
#define ATA_SR_ERR      0x01    // Error

// Commands
#define ATA_CMD_READ_PIO    0x20
#define ATA_CMD_WRITE_PIO   0x30
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_IDENTIFY    0xEC

// Global drive information
static struct {
    bool present;
    bool lba_supported;
    uint32_t total_sectors;
    char model[41];
    char serial[21];
} drive_info = {0};

// Wait for BSY to clear
static bool ata_wait_bsy(uint32_t timeout) {
    for (uint32_t i = 0; i < timeout; i++) {
        if (!(inb(ATA_STATUS) & ATA_SR_BSY)) {
            return true;
        }
        io_wait();
    }
    return false;
}

// Wait for DRQ to set
static bool ata_wait_drq(uint32_t timeout) {
    for (uint32_t i = 0; i < timeout; i++) {
        if (inb(ATA_STATUS) & ATA_SR_DRQ) {
            return true;
        }
        io_wait();
    }
    return false;
}

// Poll for status (BSY clear, DRQ set)
static bool ata_poll(uint32_t timeout) {
    if (!ata_wait_bsy(timeout)) {
        return false;
    }
    if (!ata_wait_drq(timeout)) {
        return false;
    }
    return true;
}

// Initialize ATA controller
bool ata_init(void) {
    print_string("Initializing ATA controller...\n");
    
    // Select master drive
    outb(ATA_DRIVE_SEL, 0xA0);
    
    // Wait for BSY to clear
    if (!ata_wait_bsy(5000)) {
        print_string("ATA: Drive busy timeout\n");
        return false;
    }
    
    // Check if drive exists
    outb(ATA_SECTOR_CNT, 0);
    outb(ATA_LBA_LOW, 0);
    outb(ATA_LBA_MID, 0);
    outb(ATA_LBA_HIGH, 0);
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);
    
    // Check if drive responded
    if (inb(ATA_STATUS) == 0) {
        print_string("ATA: No drive detected\n");
        return false;
    }
    
    // Wait for BSY to clear
    if (!ata_wait_bsy(5000)) {
        print_string("ATA: Identify command timeout\n");
        return false;
    }
    
    // Check for error
    uint8_t status = inb(ATA_STATUS);
    if (status & ATA_SR_ERR) {
        print_string("ATA: Error during identify\n");
        return false;
    }
    
    // Wait for DRQ
    if (!ata_wait_drq(5000)) {
        print_string("ATA: No data from drive\n");
        return false;
    }
    
    // Read identify data
    uint16_t identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inw(ATA_DATA);
    }
    
    // Parse drive information
    drive_info.present = true;
    
    // Get model string (bytes 27-46, word-swapped)
    for (int i = 0; i < 20; i++) {
        drive_info.model[i] = identify_data[27 + i] >> 8;
        drive_info.model[i + 20] = identify_data[27 + i] & 0xFF;
    }
    drive_info.model[40] = '\0';
    
    // Trim spaces from model
    for (int i = 39; i >= 0 && drive_info.model[i] == ' '; i--) {
        drive_info.model[i] = '\0';
    }
    
    // Get serial string (bytes 10-19, word-swapped)
    for (int i = 0; i < 10; i++) {
        drive_info.serial[i * 2] = identify_data[10 + i] >> 8;
        drive_info.serial[i * 2 + 1] = identify_data[10 + i] & 0xFF;
    }
    drive_info.serial[20] = '\0';
    
    // Check LBA support
    drive_info.lba_supported = (identify_data[49] & (1 << 9)) != 0;
    
    // Get total sectors (LBA28)
    if (drive_info.lba_supported) {
        drive_info.total_sectors = 
            ((uint32_t)identify_data[61] << 16) | identify_data[60];
    } else {
        // CHS geometry
        uint16_t cylinders = identify_data[1];
        uint16_t heads = identify_data[3];
        uint16_t sectors = identify_data[6];
        drive_info.total_sectors = cylinders * heads * sectors;
    }
    
    // Print drive info
    print_string("ATA: Drive detected - ");
    print_string(drive_info.model);
    print_string("\n");
    
    if (drive_info.lba_supported) {
        print_string("ATA: LBA supported, ");
        char sectors_str[16];
        utoa(drive_info.total_sectors, sectors_str, 10);
        print_string(sectors_str);
        print_string(" sectors\n");
    } else {
        print_string("ATA: CHS mode only\n");
    }
    
    return true;
}

// Read single sector (LBA)
bool disk_read_sector(uint32_t lba, uint8_t* buffer) {
    if (!drive_info.present) {
        return false;
    }
    
    // Select drive (LBA mode, master)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Wait for BSY to clear
    if (!ata_wait_bsy(5000)) {
        return false;
    }
    
    // Send sector count (1)
    outb(ATA_SECTOR_CNT, 1);
    
    // Send LBA
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send read command
    outb(ATA_COMMAND, ATA_CMD_READ_PIO);
    
    // Wait for data
    if (!ata_poll(5000)) {
        return false;
    }
    
    // Check for errors
    uint8_t status = inb(ATA_STATUS);
    if (status & ATA_SR_ERR) {
        return false;
    }
    
    // Read data (256 words = 512 bytes)
    uint16_t* buf16 = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        buf16[i] = inw(ATA_DATA);
    }
    
    return true;
}

// Write single sector (LBA)
bool disk_write_sector(uint32_t lba, uint8_t* buffer) {
    if (!drive_info.present) {
        return false;
    }
    
    // Select drive (LBA mode, master)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Wait for BSY to clear
    if (!ata_wait_bsy(5000)) {
        return false;
    }
    
    // Send sector count (1)
    outb(ATA_SECTOR_CNT, 1);
    
    // Send LBA
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send write command
    outb(ATA_COMMAND, ATA_CMD_WRITE_PIO);
    
    // Wait for DRQ
    if (!ata_wait_drq(5000)) {
        return false;
    }
    
    // Write data
    uint16_t* buf16 = (uint16_t*)buffer;
    for (int i = 0; i < 256; i++) {
        outw(ATA_DATA, buf16[i]);
    }
    
    // Flush cache
    outb(ATA_COMMAND, ATA_CMD_CACHE_FLUSH);
    
    // Wait for BSY to clear
    return ata_wait_bsy(5000);
}

// Read multiple sectors
bool disk_read_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    for (uint8_t i = 0; i < count; i++) {
        if (!disk_read_sector(lba + i, buffer + (i * 512))) {
            return false;
        }
    }
    return true;
}

// Write multiple sectors
bool disk_write_sectors(uint32_t lba, uint8_t count, uint8_t* buffer) {
    for (uint8_t i = 0; i < count; i++) {
        if (!disk_write_sector(lba + i, buffer + (i * 512))) {
            return false;
        }
    }
    return true;
}

// Get drive information
bool ata_get_drive_info(char* model, char* serial, uint32_t* sectors) {
    if (!drive_info.present) {
        return false;
    }
    
    if (model) {
        strcpy(model, drive_info.model);
    }
    if (serial) {
        strcpy(serial, drive_info.serial);
    }
    if (sectors) {
        *sectors = drive_info.total_sectors;
    }
    
    return true;
}

// Check if drive is ready
bool ata_drive_ready(void) {
    return drive_info.present;
}
