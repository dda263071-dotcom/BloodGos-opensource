# BloodG OS - Minimal x86 Educational Operating System

A clean, minimal 32-bit x86 operating system built from scratch for learning purposes. BloodG OS boots directly into a fully functional terminal interface with 16+ commands, designed specifically to teach operating system development fundamentals in a practical, hands-on way.

> **Note**: BloodG OS is an independently maintained educational fork that builds upon the foundation of [BloodOS](https://github.com/alzzmetth/Bloodos-opensource) with significant improvements, enhanced reliability, and additional educational features.

## ✨ Features

- **32-bit Protected Mode Kernel** - Real x86 protected mode implementation
- **Full PS/2 Keyboard Support** - Working backspace, Shift, CapsLock, Ctrl+C, Ctrl+L shortcuts
- **VGA Text Mode Interface** - 80x25 scrollable terminal with color support
- **16+ Built-in Commands** - Including functional calculator, test suite, and system utilities
- **Buffer Overflow Protection** - Safe input handling with 256-character limits
- **Comprehensive Test Suite** - Built-in system validation with `test` command
- **Educational Design** - Clean, readable code specifically structured for learning

## 🚀 Quick Startz

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install nasm gcc-multilib qemu-system-x86 make

# Fedora/RHEL
sudo dnf install nasm gcc qemu-system-x86 make
```

### Build & Run in Emulator
```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/bloodg-os.git
cd bloodg-os

# Build the OS image
make clean
make

# Run in QEMU (recommended for testing)
make run
```

### Create Bootable Media (Advanced)
```bash
# WARNING: Use a dedicated USB drive, not your main storage!
# Replace /dev/sdX with your USB device (check with lsblk)
sudo dd if=bloodg.img of=/dev/sdX bs=4M status=progress
sync
```

## 📁 Project Structure

```
bloodg-os/
├── boot.asm              # 16-bit bootloader (real mode)
├── kernel_entry.asm      # Kernel entry point (switches to protected mode)
├── kernel.c              # Main kernel with terminal, keyboard, commands
├── linker.ld             # Linker script for kernel layout
├── Makefile              # Build system
└── README.md             # This file
```

## ⌨️ Available Commands

| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show all available commands | `help` |
| `clear` / `cls` | Clear the terminal screen | `clear` |
| `echo <text>` | Print text to the screen | `echo Hello World` |
| `calc <num1> <op> <num2>` | Simple calculator | `calc 5 + 3` |
| `color <0-15>` | Change terminal text color | `color 14` (yellow) |
| `ver` | Show system version information | `ver` |
| `about` | Show information about BloodG OS | `about` |
| `test` | Run comprehensive system tests | `test` |
| `mem` | Show memory information | `mem` |
| `ls` | List directories (placeholder) | `ls` |
| `time` | Show system time (placeholder) | `time` |
| `date` | Show system date (placeholder) | `date` |
| `reboot` | Restart the system | `reboot` |
| `shutdown` | Power off the system | `shutdown` |
| `exit` | Exit terminal session | `exit` |

## 🛡️ Safety Features

- **Buffer Overflow Protection** - Input limited to 256 characters with automatic clearing
- **Ctrl+C Support** - Cancel any input (Unix-like behavior)
- **Ctrl+L Support** - Quick screen clearing
- **Safe Backspace Implementation** - Works correctly in all cursor positions
- **Input Validation** - Prevents system crashes on invalid input
- **Emulator-First Design** - Always test in QEMU before real hardware

## 🧪 Testing

BloodG OS includes a comprehensive built-in test suite:

```bash
bloodg> test
=== BloodG OS System Test ===
1. Backspace Test: [PASS]
2. Buffer Test: [PASS]
3. Color Test: [PASS]
4. Scroll Test: [PASS]
=== ALL TESTS PASSED ===
```

The test suite validates:
- Keyboard input and backspace functionality
- Buffer management and overflow protection
- Terminal color and display capabilities
- Screen scrolling behavior

## 🔧 Technical Details

### Boot Process
1. BIOS loads 512-byte bootloader from sector 0
2. Bootloader switches from real mode to protected mode
3. Kernel loaded at memory address 0x1000
4. Kernel initializes VGA, keyboard, and terminal
5. Command-line interface starts automatically

### Memory Map
```
0x00000000 - 0x0000FFFF: Real mode (not used after boot)
0x00010000 - 0x0008FFFF: Kernel space
0x00090000 - 0x0009FFFF: Stack space
0x000B8000 - 0x000B8FA0: VGA text buffer
```

### Keyboard Implementation
- PS/2 keyboard driver in polling mode (no interrupts needed)
- Full scancode set 1 support
- Shift, CapsLock, Ctrl, Alt state tracking
- Backspace with proper cursor handling

## ⚠️ Important Notes

### For Educational Use Only
BloodG OS is designed **exclusively for educational purposes**. It is not suitable for production use and lacks essential features for daily operation.

### Safety Recommendations
1. **Always test in QEMU first** before attempting to run on real hardware
2. **Use a dedicated USB drive** - never write to your main storage device
3. **Backup important data** before testing on real hardware
4. **Disable Secure Boot** in BIOS if testing on real hardware
5. **Have a recovery USB** ready with a working OS

### Limitations
- No filesystem support
- No memory management (paging/malloc)
- No hardware abstraction layer
- No network support
- No power management
- No user accounts or security

## 🤝 Contributing

Contributions are welcome! BloodG OS is an educational project and improvements that enhance learning value are particularly appreciated.

### Areas for Improvement
1. **Filesystem Support** - Implement FAT12 or simple RAM filesystem
2. **Memory Management** - Add paging and heap allocation
3. **Additional Commands** - Text editor, file operations, games
4. **Documentation** - Tutorials, architecture explanations
5. **Testing** - More comprehensive test cases

### Contribution Guidelines
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement`)
3. Commit changes (`git commit -am 'Add some feature'`)
4. Push to branch (`git push origin feature/improvement`)
5. Create a Pull Request

## 📚 Learning Resources

### For BloodG OS Developers
- Study `boot.asm` to understand real mode and protected mode switching
- Examine `kernel.c` to see system initialization and command structure
- Review the keyboard driver in `poll_keyboard()` for input handling


## 🙏 Acknowledgments

BloodG OS builds upon the foundation of [BloodOS](https://github.com/alzzmetth/Bloodos-opensource) by alzzmetth, whose minimal implementation provided excellent educational value. We've extended and improved the system with:

- Fixed keyboard driver with proper backspace and modifier key support
- Enhanced command system with argument parsing
- Functional calculator implementation
- Comprehensive test suite
- Buffer overflow protection
- Improved documentation and safety features

Special thanks to the OS development community for invaluable resources and inspiration.



## 📞 Support

For questions, issues, or discussions about BloodG OS:

1. **Check existing issues** on GitHub
2. **Review the documentation** above thoroughly
3. **Search OSDev resources** for general OS development questions

Remember that BloodG OS is an **educational project** maintained by volunteers. Response times may vary, but all constructive feedback is appreciated.

---
```bash
thanks for @alzzmetth
links : https://github.com/alzzmetth/Bloodos-opensource
follow : https://github.com/alzzmetth
```
**Happy coding and learning!** 🖥️💻🔧

*"The best way to understand an operating system is to build one."*
