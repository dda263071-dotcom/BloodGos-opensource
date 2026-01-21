# BloodG OS Makefile with Filesystem Support

# Tools
AS = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy
QEMU = qemu-system-x86_64
PYTHON = python3

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -fno-stack-protector -Wall -Wextra -I.
LDFLAGS = -m elf_i386 -T Linker.ld -nostdlib

# Directories
BOOT_DIR = boot
KERNEL_DIR = kernel
FS_DIR = fs
SRC_DIR = src
TOOLS_DIR = tools
BUILD_DIR = build

# Targets
TARGET = bloodg.iso
KERNEL = kernel.bin
BOOTLOADER = bootloader.bin
DISK_IMG = bloodg.img

# Object files
BOOT_OBJS = $(BUILD_DIR)/boot.o $(BUILD_DIR)/kernel_entry.o \
            $(BUILD_DIR)/shutdown.o $(BUILD_DIR)/false.o

KERNEL_OBJS = $(BUILD_DIR)/kernel.o $(BUILD_DIR)/driver.o $(BUILD_DIR)/loading.o \
              $(BUILD_DIR)/string.o $(BUILD_DIR)/io.o $(BUILD_DIR)/memory.o \
              $(BUILD_DIR)/ata.o $(BUILD_DIR)/fat12.o

# Default target
all: $(DISK_IMG) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Bootloader assembly files
$(BUILD_DIR)/boot.o: $(BOOT_DIR)/boot.asm
	$(AS) -f bin $< -o $@

$(BUILD_DIR)/kernel_entry.o: $(BOOT_DIR)/kernel_entry.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/shutdown.o: $(BOOT_DIR)/shutdown.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/false.o: $(BOOT_DIR)/false.asm
	$(AS) $(ASFLAGS) $< -o $@

# Kernel C files
$(BUILD_DIR)/kernel.o: $(KERNEL_DIR)/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/driver.o: $(KERNEL_DIR)/driver.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/loading.o: $(KERNEL_DIR)/loading.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ata.o: $(KERNEL_DIR)/ata.c
	$(CC) $(CFLAGS) -c $< -o $@

# Filesystem files
$(BUILD_DIR)/fat12.o: $(FS_DIR)/fat12.c
	$(CC) $(CFLAGS) -c $< -o $@

# Source library files
$(BUILD_DIR)/string.o: $(SRC_DIR)/string.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/io.o: $(SRC_DIR)/io.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/memory.o: $(SRC_DIR)/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL): $(BUILD_DIR) $(KERNEL_OBJS) $(BOOT_OBJS)
	$(LD) $(LDFLAGS) $(KERNEL_OBJS) $(BUILD_DIR)/kernel_entry.o \
		$(BUILD_DIR)/shutdown.o $(BUILD_DIR)/false.o -o $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel.elf $@
	@echo "Kernel size: $$(stat -f%z $@ 2>/dev/null || stat -c%s $@) bytes"

# Create FAT12 disk image
$(DISK_IMG):
	@echo "Creating FAT12 disk image..."
	$(PYTHON) $(TOOLS_DIR)/create_fat12.py $@

# Create bootable ISO
$(TARGET): $(KERNEL)
	@echo "Creating bootable ISO..."
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/
	cp $(DISK_IMG) iso/boot/disk.img 2>/dev/null || true
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo 'menuentry "BloodG OS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/kernel.bin' >> iso/boot/grub/grub.cfg
	echo '  module /boot/disk.img' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso/

# Run in QEMU with disk
run: $(DISK_IMG) $(KERNEL)
	$(QEMU) -kernel $(KERNEL) -fda $(DISK_IMG) -m 64M -serial stdio

# Run in QEMU with ISO
run-iso: $(TARGET)
	$(QEMU) -cdrom $(TARGET) -m 64M -serial stdio

# Run with debug
debug: $(KERNEL) $(DISK_IMG)
	$(QEMU) -kernel $(KERNEL) -fda $(DISK_IMG) -s -S &
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(BUILD_DIR)/kernel.elf"

# Run memory check tool
check:
	$(PYTHON) $(TOOLS_DIR)/memory_check.py

# Generate documentation
docs:
	$(PYTHON) $(TOOLS_DIR)/docs.py

# Clean build
clean:
	rm -rf $(BUILD_DIR) iso $(KERNEL) $(TARGET) $(DISK_IMG) *.log DOCUMENTATION.md

# Help
help:
	@echo "BloodG OS Build System"
	@echo "======================"
	@echo "Targets:"
	@echo "  all       - Build everything (default)"
	@echo "  run       - Run with disk image in QEMU"
	@echo "  run-iso   - Run with ISO in QEMU"
	@echo "  debug     - Run with debugger"
	@echo "  check     - Run memory check tool"
	@echo "  docs      - Generate documentation"
	@echo "  clean     - Clean build files"
	@echo ""
	@echo "Files created:"
	@echo "  kernel.bin  - Raw kernel binary"
	@echo "  bloodg.img  - FAT12 disk image"
	@echo "  bloodg.iso  - Bootable ISO"

.PHONY: all run run-iso debug check docs clean help
