/**************************************************************
 * ATA PIO Driver for BloodG OS
 * Reads/writes sectors from hard disk
 **************************************************************/

#include <stdint.h>
#include <stdbool.h>

// ATA Registers
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

// Status Register Bits
#define ATA_SR_BSY      0x80    // Busy
#define ATA_SR_DRDY     0x40    // Drive ready
#define ATA_SR_DF       0x20    // Drive write fault
#define ATA_SR_DSC      0x10    // Drive seek complete
#define ATA_SR_DRQ      0x08    // Data request ready
#define ATA_SR_CORR     0x04    // Corrected data
#define ATA_SR_IDX      0x02    // Index
#define ATA_SR_ERR      0x01    // Error

// Commands
#define ATA_CMD_READ    0x20
#define ATA_CMD_WRITE   0x30
#define ATA_CMD_IDENTIFY 0xEC

// I/O Functions
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

// Wait for BSY to clear
bool ata_wait_bsy(void) {
    uint8_t status;
    for(int i = 0; i < 100000; i++) {
        status = inb(ATA_STATUS);
        if(!(status & ATA_SR_BSY)) return true;
    }
    return false;
}

// Wait for DRQ to set
bool ata_wait_drq(void) {
    uint8_t status;
    for(int i = 0; i < 100000; i++) {
        status = inb(ATA_STATUS);
        if(status & ATA_SR_DRQ) return true;
    }
    return false;
}

// Initialize ATA controller
bool ata_init(void) {
    // Select master drive
    outb(ATA_DRIVE_SEL, 0xA0);
    
    // Wait for BSY to clear
    if(!ata_wait_bsy()) return false;
    
    // Check if drive exists
    outb(ATA_SECTOR_CNT, 0);
    outb(ATA_LBA_LOW, 0);
    outb(ATA_LBA_MID, 0);
    outb(ATA_LBA_HIGH, 0);
    outb(ATA_COMMAND, ATA_CMD_IDENTIFY);
    
    if(inb(ATA_STATUS) == 0) return false; // No drive
    
    // Wait for BSY to clear
    if(!ata_wait_bsy()) return false;
    
    // Check for error
    uint8_t status = inb(ATA_STATUS);
    if(status & ATA_SR_ERR) return false;
    
    // Wait for DRQ
    if(!ata_wait_drq()) return false;
    
    // Read identify data (discard for now)
    for(int i = 0; i < 256; i++) {
        inw(ATA_DATA);
    }
    
    return true;
}

// Read a sector (LBA addressing)
bool disk_read_sector(uint32_t lba, uint8_t* buffer) {
    // Select drive (LBA mode, master drive)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send sector count
    outb(ATA_SECTOR_CNT, 1);
    
    // Send LBA
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send read command
    outb(ATA_COMMAND, ATA_CMD_READ);
    
    // Wait for BSY to clear
    if(!ata_wait_bsy()) return false;
    
    // Check for errors
    uint8_t status = inb(ATA_STATUS);
    if(status & ATA_SR_ERR) return false;
    
    // Wait for DRQ
    if(!ata_wait_drq()) return false;
    
    // Read data (256 words = 512 bytes)
    uint16_t* buf16 = (uint16_t*)buffer;
    for(int i = 0; i < 256; i++) {
        buf16[i] = inw(ATA_DATA);
    }
    
    return true;
}

// Write a sector (LBA addressing)
bool disk_write_sector(uint32_t lba, uint8_t* buffer) {
    // Select drive (LBA mode, master drive)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    
    // Send sector count
    outb(ATA_SECTOR_CNT, 1);
    
    // Send LBA
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    
    // Send write command
    outb(ATA_COMMAND, ATA_CMD_WRITE);
    
    // Wait for DRQ
    if(!ata_wait_drq()) return false;
    
    // Write data
    uint16_t* buf16 = (uint16_t*)buffer;
    for(int i = 0; i < 256; i++) {
        outw(ATA_DATA, buf16[i]);
    }
    
    // Flush cache
    outb(ATA_COMMAND, 0xE7);
    
    // Wait for BSY to clear
    return ata_wait_bsy();
}
