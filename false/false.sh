#!/bin/bash
# BloodG OS Build Failure Handler
# Displays error messages instead of crashing

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${RED}╔══════════════════════════════════════════════════════════╗${NC}"
echo -e "${RED}║                 BLOODG OS BUILD FAILED                   ║${NC}"
echo -e "${RED}╚══════════════════════════════════════════════════════════╝${NC}"
echo ""

# Get error message
ERROR_MSG="${1:-Unknown error}"
ERROR_STAGE="${2:-Build system}"

echo -e "${YELLOW}Error Stage:${NC} $ERROR_STAGE"
echo -e "${YELLOW}Error Message:${NC} $ERROR_MSG"
echo ""

# Diagnostic information
echo -e "${BLUE}System Information:${NC}"
echo "  Date: $(date)"
echo "  Host: $(uname -a)"
echo "  Directory: $(pwd)"
echo "  User: $(whoami)"
echo ""

# Check for common issues
echo -e "${BLUE}Common Issues Check:${NC}"

# Check NASM
if ! command -v nasm &> /dev/null; then
    echo -e "  ${RED}✗ NASM not found${NC}"
else
    echo -e "  ${GREEN}✓ NASM found${NC}"
fi

# Check GCC
if ! command -v gcc &> /dev/null; then
    echo -e "  ${RED}✗ GCC not found${NC}"
else
    echo -e "  ${GREEN}✓ GCC found${NC}"
fi

# Check multilib for 32-bit
if [ -f "/usr/include/gnu/stubs-32.h" ]; then
    echo -e "  ${GREEN}✓ 32-bit libraries found${NC}"
else
    echo -e "  ${YELLOW}⚠ 32-bit libraries may be missing${NC}"
fi

# Check QEMU
if ! command -v qemu-system-x86_64 &> /dev/null; then
    echo -e "  ${YELLOW}⚠ QEMU not found (required for testing)${NC}"
else
    echo -e "  ${GREEN}✓ QEMU found${NC}"
fi

echo ""
echo -e "${BLUE}Recommended Fixes:${NC}"

case "$ERROR_STAGE" in
    "Assembly")
        echo "  1. Check NASM installation: sudo apt install nasm"
        echo "  2. Verify assembly syntax in boot/*.asm"
        echo "  3. Check for missing labels or directives"
        ;;
    "Compilation")
        echo "  1. Install 32-bit libraries: sudo apt install gcc-multilib"
        echo "  2. Check for syntax errors in C files"
        echo "  3. Verify include paths in Makefile"
        ;;
    "Linking")
        echo "  1. Check linker script (Linker.ld)"
        echo "  2. Verify all object files exist in build/"
        echo "  3. Check for duplicate symbols"
        ;;
    "Filesystem")
        echo "  1. Check Python installation for create_fat12.py"
        echo "  2. Verify disk image creation permissions"
        ;;
    *)
        echo "  1. Run './Build.sh --clean' and try again"
        echo "  2. Check file permissions in the project"
        echo "  3. Verify all required files exist"
        ;;
esac

echo ""
echo -e "${GREEN}To rebuild:${NC}"
echo "  ./Build.sh --clean"
echo "  ./Build.sh --build"
echo "  ./Build.sh --run"

echo ""
echo -e "${RED}Exiting with error code 1${NC}"
exit 1
