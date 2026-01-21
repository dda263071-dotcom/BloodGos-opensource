#!/bin/bash
#
# BloodG OS Build Script v2.1
# Complete build, test, and run script

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
KERNEL_NAME="kernel.bin"
DISK_NAME="bloodg.img"
ISO_NAME="bloodg.iso"

# Print colored message
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${CYAN}»${NC} $1"
}

# Check dependencies
check_dependencies() {
    print_info "Checking dependencies..."
    
    local deps=("nasm" "gcc" "ld" "grub-mkrescue" "qemu-system-x86_64" "python3")
    local missing=()
    
    for dep in "${deps[@]}"; do
        if ! command -v "$dep" &> /dev/null; then
            missing+=("$dep")
        fi
    done
    
    if [ ${#missing[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing[*]}"
        echo ""
        echo "Install with:"
        echo "  Ubuntu/Debian: sudo apt install nasm gcc-multilib grub-pc-bin qemu-system-x86 python3"
        echo "  Fedora/RHEL:   sudo dnf install nasm gcc ld grub2-tools qemu-system-x86 python3"
        echo "  Arch:          sudo pacman -S nasm gcc-multilib grub qemu-desktop python"
        exit 1
    fi
    
    print_success "All dependencies found"
}

# Clean build
clean_build() {
    print_info "Cleaning build..."
    make clean 2>/dev/null || true
    print_success "Clean completed"
}

# Build kernel
build_kernel() {
    print_step "Building kernel..."
    
    if ! make $KERNEL_NAME; then
        print_error "Kernel build failed"
        exit 1
    fi
    
    local kernel_size=$(stat -f%z "$KERNEL_NAME" 2>/dev/null || stat -c%s "$KERNEL_NAME" 2>/dev/null)
    print_success "Kernel built: $kernel_size bytes"
}

# Create disk image
create_disk() {
    print_step "Creating FAT12 disk image..."
    
    if [ ! -f "tools/create_fat12.py" ]; then
        print_error "Disk creation tool not found"
        return 1
    fi
    
    python3 tools/create_fat12.py "$DISK_NAME"
    
    if [ -f "$DISK_NAME" ]; then
        local disk_size=$(stat -f%z "$DISK_NAME" 2>/dev/null || stat -c%s "$DISK_NAME" 2>/dev/null)
        print_success "Disk created: $disk_size bytes"
    else
        print_warning "Disk creation may have failed"
    fi
}

# Create ISO
create_iso() {
    print_step "Creating bootable ISO..."
    
    if ! make $ISO_NAME; then
        print_error "ISO creation failed"
        return 1
    fi
    
    if [ -f "$ISO_NAME" ]; then
        local iso_size=$(stat -f%z "$ISO_NAME" 2>/dev/null || stat -c%s "$ISO_NAME" 2>/dev/null)
        print_success "ISO created: $iso_size bytes"
    fi
}

# Run tests
run_tests() {
    print_info "Running tests..."
    
    local passed=0
    local total=4
    
    # Test 1: Kernel binary
    if [ -f "$KERNEL_NAME" ]; then
        print_success "✓ Kernel binary exists"
        passed=$((passed + 1))
    else
        print_error "✗ Kernel binary missing"
    fi
    
    # Test 2: Disk image
    if [ -f "$DISK_NAME" ]; then
        print_success "✓ Disk image exists"
        passed=$((passed + 1))
    else
        print_warning "⚠ Disk image missing (use --create-disk)"
    fi
    
    # Test 3: Memory check tool
    if python3 -c "import sys" 2>/dev/null; then
        print_success "✓ Python available"
        passed=$((passed + 1))
    fi
    
    # Test 4: Build directory
    if [ -d "$BUILD_DIR" ]; then
        print_success "✓ Build directory exists"
        passed=$((passed + 1))
    fi
    
    print_info "Tests passed: $passed/$total"
    
    if [ $passed -eq $total ]; then
        print_success "All tests passed!"
    else
        print_warning "Some tests failed or skipped"
    fi
}

# Run in QEMU
run_qemu() {
    print_info "Starting BloodG OS in QEMU..."
    
    local qemu_args=()
    
    if [ -f "$DISK_NAME" ] && [ -f "$KERNEL_NAME" ]; then
        print_step "Running with kernel + disk image..."
        qemu_args=("-kernel" "$KERNEL_NAME" "-fda" "$DISK_NAME")
    elif [ -f "$ISO_NAME" ]; then
        print_step "Running with ISO image..."
        qemu_args=("-cdrom" "$ISO_NAME")
    else
        print_error "No bootable image found"
        echo "Run with --build --create-disk first"
        exit 1
    fi
    
    # Common QEMU arguments
    qemu_args+=("-m" "64M")
    qemu_args+=("-serial" "stdio")
    qemu_args+=("-no-reboot")
    qemu_args+=("-monitor" "none")
    
    echo ""
    echo "Starting QEMU with:"
    echo "  qemu-system-x86_64 ${qemu_args[*]}"
    echo ""
    echo "Useful QEMU commands:"
    echo "  Ctrl+A, X  - Exit QEMU"
    echo "  Ctrl+A, C  - Enter monitor"
    echo "  Ctrl+A, H  - Help"
    echo ""
    
    qemu-system-x86_64 "${qemu_args[@]}"
}

# Generate documentation
generate_docs() {
    print_step "Generating documentation..."
    
    if python3 tools/docs.py; then
        if [ -f "DOCUMENTATION.md" ]; then
            local doc_size=$(stat -f%z "DOCUMENTATION.md" 2>/dev/null || stat -c%s "DOCUMENTATION.md")
            print_success "Documentation generated: $doc_size bytes"
        fi
    else
        print_warning "Documentation generation had issues"
    fi
}

# Show help
show_help() {
    echo "BloodG OS Build Script v2.1"
    echo "==========================="
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  --clean         Clean build directory"
    echo "  --build         Build kernel only"
    echo "  --create-disk   Create FAT12 disk image"
    echo "  --create-iso    Create bootable ISO"
    echo "  --test          Run tests"
    echo "  --run           Run in QEMU"
    echo "  --docs          Generate documentation"
    echo "  --all           Complete build and run (default)"
    echo "  --help          Show this help"
    echo ""
    echo "Examples:"
    echo "  $0 --all               # Complete build and run"
    echo "  $0 --build --run       # Build and run"
    echo "  $0 --create-disk       # Create disk image only"
    echo ""
    echo "Features in v2.1:"
    echo "  ✓ FAT12 filesystem support"
    echo "  ✓ ATA PIO disk driver"
    echo "  ✓ Real file reading (ls, cat)"
    echo "  ✓ Professional build system"
}

# Main function
main() {
    # Check dependencies
    check_dependencies
    
    # Default action
    if [ $# -eq 0 ]; then
        print_info "No arguments specified, running complete build..."
        clean_build
        build_kernel
        create_disk
        create_iso
        run_tests
        print_success "Build completed successfully!"
        echo ""
        read -p "Run in QEMU? [Y/n] " -n 1 -r
        echo ""
        if [[ $REPLY =~ ^[Yy]$ ]] || [[ -z $REPLY ]]; then
            run_qemu
        fi
        return 0
    fi
    
    # Parse arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                clean_build
                shift
                ;;
            --build)
                build_kernel
                shift
                ;;
            --create-disk)
                create_disk
                shift
                ;;
            --create-iso)
                create_iso
                shift
                ;;
            --test)
                run_tests
                shift
                ;;
            --run)
                run_qemu
                shift
                ;;
            --docs)
                generate_docs
                shift
                ;;
            --all)
                print_info "Starting complete build process..."
                clean_build
                build_kernel
                create_disk
                create_iso
                run_tests
                generate_docs
                print_success "Complete build finished!"
                echo ""
                print_info "To run: ./Build.sh --run"
                shift
                ;;
            --help)
                show_help
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                show_help
                exit 1
                ;;
        esac
    done
}

# Run main function
main "$@"
