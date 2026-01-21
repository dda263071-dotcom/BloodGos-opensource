# 🩸 BloodG OS - Professional Operating System

<div align="center">
**A complete, modular x86 operating system with filesystem support**

</div>

## 🎯 Overview

**BloodG OS** is a professional-grade, educational x86 operating system written from scratch in C and Assembly. It features a complete bootloader, kernel, drivers, and FAT12 filesystem support. Designed for learning and experimentation, BloodG OS demonstrates real operating system concepts in a clean, modular codebase.

## ✨ Features

### ✅ Core Features
- **Custom Bootloader** - Real mode bootloader with protected mode transition
- **32-bit Protected Mode Kernel** - Full 32-bit operation with GDT setup
- **FAT12 Filesystem** - Complete read/write support with cluster allocation
- **ATA PIO Driver** - Hard disk access using Programmed I/O
- **VGA Text Mode** - 80x25 terminal with color support
- **PS/2 Keyboard** - Full keyboard driver with scancode translation
- **PIC Interrupt Controller** - Hardware interrupt handling

### 🚀 Advanced Capabilities
- **Memory Management** - Bump allocator with 1MB kernel pool
- **Serial Port Support** - COM1 debugging and communication
- **PIT Timer** - Programmable Interval Timer for scheduling
- **Modular Architecture** - Clean separation between kernel, drivers, and libraries
- **Build System** - Professional Makefile with automated testing
- **Development Tools** - Python scripts for disk creation and memory checking

### 📁 Filesystem Operations
- `ls` - List directory contents
- `cat` - Display file contents
- File reading with cluster chain following
- Directory parsing and metadata display

## 🏗️ Project Structure

```
bloodg-os/
├── boot/                    # Bootloader components
│   ├── boot.asm            # Main bootloader (Real mode → Protected mode)
│   ├── kernel_entry.asm    # Kernel entry point with stack setup
│   ├── shutdown.asm        # System shutdown/reboot functions
│   └── false.asm           # Error handler for kernel validation
│
├── kernel/                  # Core kernel
│   ├── kernel.c            # Main kernel with shell and command processor
│   ├── loading.c           # Animated loading screen with ASCII art
│   └── driver.c            # Basic hardware I/O functions
│
├── drivers/                 # Hardware drivers
│   ├── ata.c               # ATA/IDE disk driver (PIO mode)
│   ├── keyboard.c          # PS/2 keyboard driver with scancode translation
│   ├── vga.c               # VGA text mode driver with color support
│   ├── timer.c             # PIT (Programmable Interval Timer) driver
│   ├── serial.c            # Serial port driver (COM1)
│   └── pic.c               # PIC (8259) interrupt controller
│
├── fs/                      # Filesystem implementation
│   └── fat12.c             # Complete FAT12 filesystem driver
│
├── src/                     # Core libraries
│   ├── string.c            # String and memory manipulation functions
│   ├── io.c                # Port I/O and CPU control functions
│   └── memory.c            # Memory management with 1MB pool
│
├── include/                 # Header files
│   ├── string.h            # String library declarations
│   ├── io.h                # I/O port declarations
│   ├── memory.h            # Memory management declarations
│   ├── fat12.h             # FAT12 filesystem API
│   ├── ata.h               # ATA driver interface
│   ├── keyboard.h          # Keyboard driver interface
│   ├── vga.h               # VGA text mode interface
│   ├── timer.h             # Timer functions
│   └── serial.h            # Serial port interface
│
├── tools/                   # Development tools
│   ├── create_fat12.py     # FAT12 disk image creator
│   ├── memory_check.py     # Memory layout visualization tool
│   ├── memory_check.c      # C version for kernel integration
│   └── docs.py             # Documentation generator
│
├── build/                   # Build artifacts (generated)
├── Linker.ld               # Linker script for kernel layout
├── Makefile                # Professional build system
└── Build.sh                # Automated build and test script
```

## 🚀 Getting Started

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install nasm gcc-multilib grub-pc-bin qemu-system-x86 python3

# Fedora/RHEL
sudo dnf install nasm gcc ld grub2-tools qemu-system-x86 python3

# Arch Linux
sudo pacman -S nasm gcc-multilib grub qemu-desktop python

# macOS (with Homebrew)
brew install nasm i386-elf-gcc i386-elf-binutils qemu python3
```

### Quick Start

```bash
# Clone the repository
git clone https://github.com/one_to_go/bloodg-os.git
cd bloodg-os

# Make scripts executable
chmod +x Build.sh
chmod +x tools/*.py

# Complete build and run
./Build.sh --all
```

## 🔧 Building from Source

### Complete Build Process

```bash
# Method 1: Using the build script (recommended)
./Build.sh --all

# Method 2: Step by step
./Build.sh --clean      # Clean build directory
./Build.sh --build      # Build kernel
./Build.sh --create-disk # Create FAT12 disk image
./Build.sh --test       # Run tests
./Build.sh --run        # Run in QEMU
```

### Manual Build

```bash
# Clean previous builds
make clean

# Build everything
make all

# Create disk image
python3 tools/create_fat12.py bloodg.img

# Run in QEMU
make run
```

## 💻 Running BloodG OS

### Running in QEMU

```bash
# Run with kernel + disk image
qemu-system-x86_64 -kernel kernel.bin -fda bloodg.img -m 64M -serial stdio

# Run with ISO image
qemu-system-x86_64 -cdrom bloodg.iso -m 64M -serial stdio

# Run with debugger
make debug
```

### Available Commands

Once BloodG OS boots, you'll see the `bloodg> ` prompt. Available commands:

```bash
bloodg> help              # Show all available commands
bloodg> ls                # List files on FAT12 disk
bloodg> cat README.TXT    # Display file contents
bloodg> ver               # Show BloodG OS version
bloodg> mem               # Display memory information
bloodg> about             # Show about information
bloodg> clear             # Clear screen (also: cls)
bloodg> echo <text>       # Print text
bloodg> reboot            # Restart system
bloodg> shutdown          # Power off system
```

## 🏛️ System Architecture

### Boot Process
1. **BIOS** loads bootloader at 0x7C00
2. **Bootloader** enables A20 line, loads GDT, switches to protected mode
3. **Kernel Entry** sets up stack and calls `kmain()`
4. **Kernel Initialization** initializes hardware drivers and filesystem
5. **Shell** starts interactive command processor

### Memory Layout
```
0x00000000 - 0x0000FFFF   Real Mode Area (64KB)
0x00010000 - 0x0008FFFF   Kernel Code/Data (32KB)
0x00090000 - 0x0009FFFF   Stack Space (16KB)
0x000B8000 - 0x000B8FA0   VGA Text Buffer (4KB)
0x00100000 - 0x01FFFFFF   Available Memory (31MB)
```

### Filesystem Structure
BloodG OS uses **FAT12** filesystem with:
- 512 bytes per sector
- 1 sector per cluster
- 2 FAT tables (for redundancy)
- 224 root directory entries
- Support for files up to 32MB

## 📚 Command Reference

### System Commands
| Command | Description | Usage |
|---------|-------------|-------|
| `help` | Show all commands | `help` |
| `ver` | Show version info | `ver` |
| `mem` | Display memory info | `mem` |
| `about` | About BloodG OS | `about` |
| `clear`/`cls` | Clear screen | `clear` |
| `echo` | Print text | `echo Hello World` |
| `reboot` | Restart system | `reboot` |
| `shutdown` | Power off | `shutdown` |

### Filesystem Commands
| Command | Description | Usage |
|---------|-------------|-------|
| `ls`/`dir` | List directory | `ls` |
| `cat`/`type` | Display file | `cat README.TXT` |

## 🛠️ Development

### Code Organization

```
# Kernel Development
kernel/kernel.c          # Main kernel logic
kernel/loading.c         # Loading screen animation

# Driver Development
drivers/ata.c           # Disk access
drivers/keyboard.c      # Input handling
drivers/vga.c           # Display output

# Filesystem Development
fs/fat12.c              # FAT12 implementation

# Library Development
src/string.c            # String utilities
src/io.c                # Hardware access
src/memory.c            # Memory management
```

### Adding New Features

1. **New Command**: Add to command table in `kernel/kernel.c`
2. **New Driver**: Create in `drivers/` and add to Makefile
3. **New System Call**: Extend kernel API in `include/kernel.h`
4. **New Filesystem Feature**: Modify `fs/fat12.c`

### Debugging

```bash
# Run with GDB debugger
make debug

# View memory map
python3 tools/memory_check.py

# Generate documentation
python3 tools/docs.py

# Create custom disk image
python3 tools/create_fat12.py custom.img
```

## 🤝 Contributing

We welcome contributions to BloodG OS! Here's how you can help:

### Reporting Issues
- Use GitHub Issues to report bugs
- Include QEMU version and host OS
- Provide steps to reproduce

### Submitting Changes
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

### Development Guidelines
- Follow existing code style
- Add comments for complex logic
- Update documentation
- Test with both QEMU and real hardware (if possible)

### Areas Needing Help
- Adding ext2 filesystem support
- Implementing paging and virtual memory
- Adding USB driver support
- Creating graphical user interface
- Porting to ARM architecture
Assembly:        205 baris
Kernel C:        820 baris
Drivers C:     1,630 baris
Filesystem C:    500 baris
Libraries C:     520 baris
Headers:         540 baris
Tools:         1,345 baris
TOTAL:         5,560 baris



### Special Thanks
- All contributors who have helped shape BloodG OS
- The open source community for inspiration and tools
- Early testers who provided valuable feedback

## 📞 Contact & Support

**Developer**: @one_to_go  
**Repository**: https://github.com/dda263071-dotcom/BloodGos-opensource
**Issues**: https://github.com//dda263071-dotcom/BloodGos-opensourceissues  


---

<div align="center">
  
## Credits
- Thanks to **@alzzmetth**
- I'm a contributor of **blood os**

## Repository
🔗 **bloodOS**: https://github.com/alzzmetth/bloodos-opensource

**"From boot sector to filesystem - an operating system journey"**

*Built with passion for learning and sharing knowledge*

⭐ **Star this project if you find it helpful!** ⭐
