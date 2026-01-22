# 🩸 BloodGos OS

> Hobby Operating System written from scratch in **C & x86 Assembly**

BloodGos adalah **hobby operating system** yang dibuat untuk pembelajaran **low-level programming dan OS development**. Project ini **BUKAN Linux fork** dan **BUKAN based on existing OS**, melainkan kernel dan bootloader buatan sendiri.

---

## ✨ Features

* Custom **Bootloader (x86)**
* **Protected Mode Kernel**
* **FAT12 File System** support
* VGA Text Mode output
* Keyboard (PS/2) Driver
* ATA Disk Driver
* Basic Memory Management
* Modular kernel structure

---



---

## 🚀 Getting Started

Tutorial berikut bisa dijalankan di **PC (Windows / Linux)** dan **HP Android**.

---

## 🖥️ Tutorial Lengkap di PC

### 🔧 1. Requirements

#### Windows

* Windows 10 / 11
* WSL (Ubuntu recommended) **atau** Linux native
* NASM
* GCC
* Make
* QEMU

#### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential nasm qemu-system-x86
```

---

### ▶️ 2. Clone Repository

```bash
git clone https://github.com/dda263071-dotcom/BloodGos-opensource.git
cd BloodGos-opensource
```

---

### ⚙️ 3. Build OS

```bash
make
```

Jika berhasil, file image / ISO akan terbuat.

---

### ▶️ 4. Run di QEMU

```bash
make run
```

Atau manual:

```bash
qemu-system-i386 -cdrom BloodGos.iso
```

---

## 📱 Tutorial Lengkap di HP (Android)

> ⚠️ Performa terbatas, tapi **cukup untuk belajar**.

### 🔧 1. Install Aplikasi

* **Termux** (via F-Droid)
* **QEMU for Android** (opsional, berat)

---

### ▶️ 2. Setup Termux

```bash
pkg update
pkg install git clang make nasm qemu-system-x86
```

---

### ▶️ 3. Clone & Build

```bash
git clone https://github.com/dda263071-dotcom/BloodGos-opensource.git
cd BloodGos-opensource
make
```

---

### ▶️ 4. Run (Jika QEMU tersedia)

```bash
qemu-system-i386 -cdrom BloodGos.iso
```

Jika tidak bisa run di HP, build tetap bisa dilakukan untuk **belajar struktur & kode OS**.

---

## 🧪 Tips Debugging

* Gunakan `printf` VGA untuk debug kernel
* Cek log QEMU jika boot gagal
* Build ulang jika edit ASM

---

## 🧠 Project Structure

Struktur **BloodG-OS** dirancang modular dan profesional agar mudah dipelajari untuk OS Development tingkat low-level.

```text
bloodg-os/
├── boot/                    # Bootloader components
│   ├── boot.asm            # Main bootloader (Real Mode → Protected Mode)
│   ├── kernel_entry.asm    # Kernel entry point + stack setup
│   ├── shutdown.asm        # System shutdown & reboot routines
│   └── false.asm           # Kernel validation & fatal error handler
│
├── kernel/                  # Core kernel
│   ├── kernel.c            # Main kernel + shell + command processor
│   ├── loading.c           # Animated loading screen (ASCII art)
│   └── driver.c            # Kernel-level I/O helpers
│
├── drivers/                 # Hardware drivers
│   ├── ata.c               # ATA / IDE disk driver (PIO)
│   ├── keyboard.c          # PS/2 keyboard + scancode translation
│   ├── vga.c               # VGA text mode driver (color support)
│   ├── timer.c             # PIT (Programmable Interval Timer)
│   ├── serial.c            # Serial port (COM1) driver
│   └── pic.c               # PIC 8259 interrupt controller
│
├── fs/                      # Filesystem layer
│   └── fat12.c             # Complete FAT12 filesystem implementation
│
├── src/                     # Core libraries
│   ├── string.c            # Custom string & memory routines
│   ├── io.c                # Port I/O & CPU instructions
│   └── memory.c            # Memory manager (1MB pool)
│
├── include/                 # Public headers
│   ├── string.h            # String API
│   ├── io.h                # Low-level I/O API
│   ├── memory.h            # Memory manager API
│   ├── fat12.h             # FAT12 filesystem API
│   ├── ata.h               # ATA interface
│   ├── keyboard.h          # Keyboard interface
│   ├── vga.h               # VGA text mode API
│   ├── timer.h             # Timer interface
│   └── serial.h            # Serial port API
│
├── tools/                   # Development utilities
│   ├── create_fat12.py     # FAT12 disk image generator
│   ├── memory_check.py     # Memory layout visualizer
│   ├── memory_check.c      # Kernel memory checker
│   └── docs.py             # Documentation generator
│
├── build/                   # Build artifacts (auto-generated)
├── Linker.ld               # Kernel linker script
├── Makefile                # Professional build system
└── Build.sh                # Automated build & test script
```

---

## 🎯 Project Goals

* Learn how an OS boots and runs on real hardware
* Understand memory management, interrupts, and drivers
* Build a clean and readable hobby OS codebase
* Become a reference project for Indonesian OSDev learners 🇮🇩

---

## 🛣️ Roadmap

* [x] Bootloader
* [x] Kernel entry
* [x] FAT12 support
* [x] Keyboard input
* [ ] Simple shell
* [ ] Paging / Virtual Memory
* [ ] User mode
* [ ] Basic multitasking

---

## 🤝 Contributing

Contributions are **very welcome**!

You can help by:

* Improving documentation
* Refactoring code
* Adding drivers or kernel features
* Fixing bugs

Check **Issues** for `good first issue` or `help wanted` labels.

---

## ❗ Disclaimer

BloodGos is an **educational hobby OS**.

❌ Not intended for production use
❌ No security guarantee
✅ Built for learning & experimentation

---

## 📚 References & Inspiration

* OSDev Wiki
* Intel x86 Manuals
* Classic hobby OS projects

---

## ⭐ Support

If you find this project interesting or useful:

* ⭐ Star this repository
* 🍴 Fork it
* 🗣️ Share it with other OSDev learners

---

**Built with passion for low-level programming ❤️**

