#!/bin/bash
echo "Building BloodOS..."

# Install dependencies (Ubuntu/Debian)
# sudo apt-get install nasm gcc-multilib qemu-system-x86

# Clean previous builds
make clean

# Build
make

# Check if build successful
if [ -f "bloodos.img" ]; then
    echo "✅ Build successful!"
    echo "Image: bloodos.img (1.44MB)"
    echo ""
    echo "To run in QEMU:"
    echo "  make run        # Floppy mode"
    echo "  make run-hdd    # Hard disk mode"
    echo ""
    echo "To create bootable USB (CAREFUL!):"
    echo "  sudo dd if=bloodos.img of=/dev/sdX bs=4M status=progress"
else
    echo "❌ Build failed!"
    exit 1
fi
