# Bloodos-opensource
BloodOS - Minimal x86 Operating System

📌 Overview

BloodOS is a minimal x86 operating system built from scratch using Assembly and C. It boots directly into a terminal interface with basic command-line functionality.

🚀 Features

· 32-bit protected mode kernel
· VGA text mode display (80x25)
· Keyboard input with backspace support
· Simple command-line interface
· Bootable from USB/floppy disk
· Safe for testing (runs in emulation or separate media)

🛠️ System Requirements

· x86-compatible processor (real or virtual)
· 64KB RAM minimum
· USB drive or floppy disk for real hardware
· QEMU for virtual testing

📁 Project Structure

```
bloodos/
├── boot.asm          # Bootloader (16-bit)
├── kernel_entry.asm  # Kernel entry point
├── kernel.c          # Main kernel
├── linker.ld         # Linker script
├── Makefile          # Build system
└── build.sh          # Build script
```

🔧 Installation & Building

1. Install Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install nasm gcc-multilib qemu-system-x86 make
```

2. Build BloodOS

```bash
# Clone or create project directory
mkdir bloodos && cd bloodos

# Place all source files in this directory
# Then build:
make
```

3. Create Bootable Media

```bash
# For USB drive (replace /dev/sdX with your USB)
sudo dd if=bloodos.img of=/dev/sdX bs=4M status=progress
sync
```

🎮 Running BloodOS

In QEMU (Virtual)

```bash
# Floppy emulation
make run

# Or hard disk emulation
qemu-system-x86_64 -hda bloodos.img
```

On Real Hardware

1. Create bootable USB as shown above
2. Set BIOS to boot from USB
3. Disable Secure Boot if enabled
4. Boot from USB

⌨️ Available Commands

```
help     - Show all commands
clear    - Clear screen (alias: cls)
echo     - Print text
reboot   - Restart system
shutdown - Power off
ver      - Show version info
color    - Change text color (0-9)
ls       - List directories
time     - Show current time
date     - Show current date
calc     - Simple calculator
mem      - Memory information
exit     - Exit terminal session
```

🎨 Display Features

· Custom ASCII art banner on boot
· Green prompt: root~bloodos:~ 
· Scrollable screen (when full)
· Backspace and Enter key support
· Color-changing capability

⚙️ Technical Details

Boot Process

1. BIOS loads bootloader (512 bytes)
2. Bootloader switches to protected mode
3. Kernel loaded at 0x1000 address
4. Kernel initializes VGA, keyboard, interrupts
5. Terminal prompt displayed

Memory Map

```
0x00000000 - 0x0000FFFF: Real mode (not used)
0x00010000 - 0x0008FFFF: Kernel space
0x00090000 - 0x0009FFFF: Stack space
0x000B8000 - 0x000B8FA0: VGA text buffer
```

Interrupts Handled

· IRQ1: Keyboard input
· No other hardware interrupts (minimal system)

⚠️ Safety Warnings

DO:

· Test in QEMU/VirtualBox first
· Use dedicated USB drive for real hardware
· Backup important data before testing
· Verify USB device name before writing

DON'T:

· Write to your main hard drive (/dev/sda)
· Install as primary operating system
· Expect full hardware compatibility
· Use on production machines

🔍 Troubleshooting

Boot Issues

```
Problem: "No bootable device"
Solution: 
  1. Check BIOS boot order
  2. Try USB 2.0 port (not 3.0)
  3. Disable Secure Boot
  4. Enable Legacy Boot/CSM mode
```

QEMU Issues

```
Problem: QEMU doesn't start
Solution: Install correct packages:
  sudo apt-get install qemu-system-x86
```

Build Issues

```
Problem: "i686-elf-gcc not found"
Solution: Install cross-compiler:
  sudo apt-get install gcc-multilib gcc-i686-linux-gnu
```

📚 Learning Resources

For Beginners

1. Start with QEMU testing only
2. Learn basic x86 Assembly
3. Understand protected mode switching
4. Study VGA text mode programming

Next Steps

1. Add file system support
2. Implement memory management
3. Add more device drivers
4. Create user space programs

🔄 Development Workflow

1. Edit Code

```bash
nano kernel.c  # Edit kernel
nano boot.asm  # Edit bootloader
```

2. Build & Test

```bash
make clean     # Clean previous builds
make           # Build new image
make run       # Test in QEMU
```

3. Deploy to USB

```bash
# After confirming QEMU works
sudo dd if=bloodos.img of=/dev/sdX bs=4M
```

🎯 Quick Start Commands

All-in-One Build & Run

```bash
# Copy-paste this entire block:
mkdir -p ~/bloodos && cd ~/bloodos
# [Paste all source files here]
make && make run
```

Clean Rebuild

```bash
make clean
make
make run
```

💡 Tips & Tricks

Keyboard Shortcuts in QEMU

· Ctrl+Alt - Release mouse/keyboard
· Ctrl+Alt+Del - Reboot VM
· Ctrl+Alt+F - Toggle fullscreen

Testing on Real Hardware

1. Use old laptop/desktop for testing
2. Remove hard drive during testing
3. Use USB hub to protect ports
4. Have rescue USB ready with Linux

Debugging

```bash
# Run QEMU with debugger
qemu-system-x86_64 -s -S -hda bloodos.img
# In another terminal: gdb -ex "target remote localhost:1234"
```

📊 Performance

Expected Results

· Boot time: < 1 second
· Memory usage: ~64KB
· Storage: 1.44MB (floppy size)
· No disk access after boot

Limitations

· No multi-tasking
· No file system
· No network support
· No sound support
· No power management

🤝 Contributing

While this is a personal project, improvements are welcome:

1. Fix bugs in keyboard handling
2. Add simple text editor
3. Implement basic file system
4. Add more terminal commands
5. Improve error messages

📞 Support

Common Issues & Solutions

Q: Screen shows garbage characters
A: VGA buffer corrupted. Check kernel memory addresses.

Q: Keyboard doesn't work
A: PIC not initialized properly. Check init_pic() function.

Q: Can't boot from USB
A: Try different USB brand, some have compatibility issues.

Q: System hangs after boot
A: Stack setup incorrect. Check kernel_entry.asm.

🔮 Future Development Ideas

Phase 1 (Current)

· Basic bootloader
· Terminal interface
· Keyboard input
· Simple file operations

Phase 2

· FAT16 file system
· Text editor
· Basic games (snake, etc.)
· Configuration files

Phase 3

· Multitasking
· User accounts
· Network stack
· Package manager

🎉 Getting Started

For Students

1. Study the boot.asm file first
2. Understand GDT setup
3. Learn how protected mode works
4. Experiment with VGA output

For Hobbyists

1. Try adding new commands
2. Change color schemes
3. Add startup messages
4. Create custom ASCII art

For Developers

1. Examine the memory layout
2. Study interrupt handling
3. Plan extended features
4. Consider porting to ARM

📝 Final Notes

BloodOS is designed as an educational tool to understand how operating systems work at the lowest level. It's not meant for daily use but as a foundation for learning OS development concepts.

Remember:

· Always test in virtual machine first
· Keep backups of your work
· Document your changes
· Have fun experimenting!

🚨 Emergency Recovery

If your system becomes unbootable:

1. Boot from Linux USB
2. Mount your main drive
3. Reinstall bootloader:
   ```bash
   sudo grub-install /dev/sda
   sudo update-grub
   ```

---

Happy Coding! 🖥️💻🔧

Note: This OS is for educational purposes only. Use at your own risk on real hardware.
notes:

kami tidak bertanggung jawab jika pc anda matot dll karna mencoba os ini karna ini bukan project serius
