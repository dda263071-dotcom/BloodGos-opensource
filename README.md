# BloodGos LibC

BloodGos LibC adalah implementasi custom C standard-like library yang dirancang khusus untuk sistem operasi BloodGos. Library ini dibuat untuk lingkungan **freestanding kernel**, sehingga tidak bergantung pada libc dari sistem host (glibc, musl, dll).

Library ini menyediakan komponen fundamental seperti:

- Manajemen memori
- Operasi string & memory
- Matematika dasar
- Kriptografi
- I/O level rendah
- Utility standar (itoa, stdio, dll)
- Timer dan hardware interaction dasar

Library ini sepenuhnya ditulis untuk kebutuhan kernel dan berjalan tanpa runtime eksternal.

---

## 🎯 Tujuan Project

BloodGos LibC dibuat untuk:

- Menyediakan fondasi standar untuk kernel BloodGos
- Menghindari ketergantungan pada libc eksternal
- Memberikan struktur modular dan scalable
- Menjadi referensi pembelajaran OS development (C + low-level system)

Library ini bersifat **educational + experimental**, namun tetap dirancang dengan struktur profesional dan modular.

---

# 📁 Struktur Direktori

```
include/libc/
├── types.h
├── timer.h
├── print_string.h
├── io.h
├── itoa.h
├── malloc.h
├── stdio.h
├── math/
│   └── math.h
├── crypto/
│   └── crypto.h
└── string/
    └── string.h

src/libc/
├── timer.c
├── print_string.c
├── io.c
├── itoa.c
├── malloc.c
├── stdio.c
├── math/
│   ├── trig.c
│   ├── sin.c
│   └── sqrt.c
├── crypto/
│   ├── rotate.c
│   ├── crc32.c
│   ├── aes.c
│   ├── sha256.c
└── string/
    ├── mem.c
    └── str.c
```

---

# 📦 Komponen Library

## 1️⃣ Core Types

### `types.h`
Menyediakan definisi tipe dasar untuk sistem freestanding:

- `uint8_t`
- `uint16_t`
- `uint32_t`
- `size_t`
- dll

Digunakan sebagai fondasi seluruh kernel.

---

## 2️⃣ Memory Management

### `malloc.h` / `malloc.c`
Implementasi memory allocator sederhana untuk kernel.

Fitur:
- Dynamic memory allocation
- Kernel heap management
- Freestanding compatible

Digunakan untuk struktur data dinamis di kernel.

---

## 3️⃣ String & Memory

### `string/string.h`
Berisi fungsi manipulasi string dan memory seperti:

- `strlen`
- `strcmp`
- `strcpy`
- `memcpy`
- `memset`
- dll

Implementasi dibagi menjadi:

- `mem.c` → fungsi memory level rendah
- `str.c` → manipulasi string karakter

Dirancang tanpa dependency libc eksternal.

---

## 4️⃣ I/O Layer

### `io.h` / `io.c`
Low-level hardware I/O:

- `inb`
- `outb`
- Port communication

Digunakan untuk komunikasi dengan perangkat seperti:
- Keyboard PS/2
- ATA
- Timer
- VGA

---

## 5️⃣ Console Output

### `print_string.h` / `print_string.c`
Fungsi untuk menampilkan teks ke layar (VGA text mode).

Digunakan sebagai backend untuk `stdio`.

---

## 6️⃣ Standard IO (Kernel Mode)

### `stdio.h` / `stdio.c`
Implementasi ringan fungsi seperti:

- `printf` (versi kernel-safe)
- Output formatting dasar

Dirancang untuk debugging dan logging kernel.

---

## 7️⃣ Timer

### `timer.h` / `timer.c`
Driver sederhana untuk programmable interval timer (PIT).

Digunakan untuk:
- Delay
- Scheduling
- Tick counter

---

## 8️⃣ Utility

### `itoa.h` / `itoa.c`
Integer to ASCII conversion.

Digunakan oleh:
- printf
- debugging output
- logging

---

## 9️⃣ Math Module

Folder: `math/`

### `math.h`
Interface utama matematika.

Implementasi:
- `sin`
- `sqrt`
- fungsi trigonometri
- operasi numerik dasar

Dirancang tanpa floating-point hardware dependency berat (kernel-friendly).

---

## 🔐 Crypto Module

Folder: `crypto/`

### `crypto.h`
Interface utama cryptographic utilities.

Implementasi:

- `rotate.c` → bit rotation
- `crc32.c` → checksum CRC32
- `aes.c` → symmetric encryption
- `sha256.c` → hashing SHA-256

Digunakan untuk:
- File verification
- Integrity check
- Future filesystem security
- Networking layer (planned)

---

# ⚙️ Build Environment

Library ini dirancang untuk:

- GCC cross-compiler (i686-elf)
- Freestanding compilation mode
- Tanpa linking libc host

Compile flags biasanya meliputi:

```
-ffreestanding
-nostdlib
-fno-builtin
```

---

# 🧠 Design Philosophy

BloodGos LibC mengikuti prinsip:

- Modular
- Low-level first
- Minimal dependency
- Kernel-oriented
- Educational clarity

Struktur dipisah antara:

- Header (interface)
- Source (implementation)
- Modul per kategori (math, crypto, string)

Hal ini mempermudah scaling dan maintenance jangka panjang.

---

# 🚀 Roadmap (Planned)

Beberapa pengembangan masa depan:

- Memory allocator yang lebih advanced (paging-aware)
- Extended printf formatter
- Random number generator
- Big integer math
- Networking crypto utilities
- Optimized assembly versions untuk fungsi kritikal

---

# 📖 Educational Purpose

Library ini dibuat sebagai bagian dari proyek OS BloodGos untuk:

- Mempelajari cara kerja libc di level rendah
- Memahami bagaimana kernel membangun fondasi runtime-nya sendiri
- Memberikan referensi open-source untuk developer OS pemula

---

# 📜 License

Project ini bersifat open-source.  
Silakan gunakan, modifikasi, dan pelajari untuk tujuan edukasi.

---

# 👨‍💻 Author

developed by : @alzzdevmaret

---

Jika kamu tertarik pada OS development, silakan eksplorasi kode, lakukan eksperimen, dan bangun sistem operasimu sendiri 🚀
