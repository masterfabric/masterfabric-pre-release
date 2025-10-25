#!/bin/bash

# MasterFabric Pre-Release Security Checker - Linux Installation Script
# Version: 1.0.0

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BINARY_NAME="masterfabric-prerelease"
INSTALL_DIR="/usr/local/bin"
PACKAGE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo -e "${BLUE}================================================================================${NC}"
echo -e "${BLUE}                    MasterFabric Pre-Release Security Checker                   ${NC}"
echo -e "${BLUE}                    Linux Installation Script v1.0.0                           ${NC}"
echo -e "${BLUE}================================================================================${NC}"
echo ""

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo -e "${RED}Error: This installation script is designed for Linux only.${NC}"
    echo -e "${YELLOW}For other platforms, please use the appropriate installation method.${NC}"
    exit 1
fi

# Check if running as root
if [[ $EUID -eq 0 ]]; then
    echo -e "${YELLOW}Warning: Running as root. This is not recommended.${NC}"
    echo -e "${YELLOW}Consider running without sudo for better security.${NC}"
    echo ""
fi

# Check if binary exists
if [[ ! -f "$PACKAGE_DIR/$BINARY_NAME" ]]; then
    echo -e "${RED}Error: Binary '$BINARY_NAME' not found in package directory.${NC}"
    echo -e "${YELLOW}Please ensure you're running this script from the correct directory.${NC}"
    exit 1
fi

# Check for required dependencies
echo -e "${BLUE}Checking system dependencies...${NC}"
MISSING_DEPS=()

# Check for libcurl
if ! pkg-config --exists libcurl 2>/dev/null; then
    MISSING_DEPS+=("libcurl4-openssl-dev")
fi

# Check for json-c
if ! pkg-config --exists json-c 2>/dev/null; then
    MISSING_DEPS+=("libjson-c-dev")
fi

# Check for curl command
if ! command -v curl >/dev/null 2>&1; then
    MISSING_DEPS+=("curl")
fi

if [[ ${#MISSING_DEPS[@]} -gt 0 ]]; then
    echo -e "${YELLOW}Missing dependencies detected:${NC}"
    for dep in "${MISSING_DEPS[@]}"; do
        echo -e "  - $dep"
    done
    echo ""
    echo -e "${BLUE}Installing missing dependencies...${NC}"
    
    # Detect package manager and install dependencies
    if command -v apt-get >/dev/null 2>&1; then
        echo -e "${BLUE}Using apt-get package manager...${NC}"
        sudo apt-get update
        sudo apt-get install -y "${MISSING_DEPS[@]}"
    elif command -v yum >/dev/null 2>&1; then
        echo -e "${BLUE}Using yum package manager...${NC}"
        sudo yum install -y "${MISSING_DEPS[@]}"
    elif command -v dnf >/dev/null 2>&1; then
        echo -e "${BLUE}Using dnf package manager...${NC}"
        sudo dnf install -y "${MISSING_DEPS[@]}"
    elif command -v pacman >/dev/null 2>&1; then
        echo -e "${BLUE}Using pacman package manager...${NC}"
        sudo pacman -S --noconfirm "${MISSING_DEPS[@]}"
    elif command -v zypper >/dev/null 2>&1; then
        echo -e "${BLUE}Using zypper package manager...${NC}"
        sudo zypper install -y "${MISSING_DEPS[@]}"
    else
        echo -e "${RED}Error: Could not detect package manager.${NC}"
        echo -e "${YELLOW}Please install the following dependencies manually:${NC}"
        for dep in "${MISSING_DEPS[@]}"; do
            echo -e "  - $dep"
        done
        exit 1
    fi
else
    echo -e "${GREEN}✓ All dependencies are installed${NC}"
fi

# Check if install directory exists and is writable
if [[ ! -d "$INSTALL_DIR" ]]; then
    echo -e "${YELLOW}Creating installation directory: $INSTALL_DIR${NC}"
    sudo mkdir -p "$INSTALL_DIR"
fi

if [[ ! -w "$INSTALL_DIR" ]]; then
    echo -e "${YELLOW}Installation directory requires elevated permissions.${NC}"
    echo -e "${BLUE}You may be prompted for your password.${NC}"
fi

# Install the binary
echo -e "${BLUE}Installing MasterFabric Security Checker...${NC}"
if sudo cp "$PACKAGE_DIR/$BINARY_NAME" "$INSTALL_DIR/"; then
    echo -e "${GREEN}✓ Binary copied successfully${NC}"
else
    echo -e "${RED}Error: Failed to copy binary to $INSTALL_DIR${NC}"
    exit 1
fi

# Make binary executable
if sudo chmod +x "$INSTALL_DIR/$BINARY_NAME"; then
    echo -e "${GREEN}✓ Binary permissions set correctly${NC}"
else
    echo -e "${RED}Error: Failed to set executable permissions${NC}"
    exit 1
fi

# Verify installation
echo -e "${BLUE}Verifying installation...${NC}"
if command -v "$BINARY_NAME" >/dev/null 2>&1; then
    INSTALLED_VERSION=$("$BINARY_NAME" --version 2>/dev/null || echo "Unknown")
    echo -e "${GREEN}✓ Installation successful!${NC}"
    echo -e "${GREEN}✓ Installed version: $INSTALLED_VERSION${NC}"
    echo ""
    echo -e "${BLUE}================================================================================${NC}"
    echo -e "${GREEN}🎉 MasterFabric Security Checker has been installed successfully! 🎉${NC}"
    echo -e "${BLUE}================================================================================${NC}"
    echo ""
    echo -e "${YELLOW}Usage Examples:${NC}"
    echo -e "  ${BLUE}$BINARY_NAME --help${NC}                    # Show help information"
    echo -e "  ${BLUE}$BINARY_NAME --version${NC}                 # Show version information"
    echo -e "  ${BLUE}$BINARY_NAME --check /path/to/project${NC}  # Scan a project directory"
    echo -e "  ${BLUE}$BINARY_NAME --check . --html report.html${NC}  # Scan and export HTML report"
    echo ""
    echo -e "${YELLOW}Documentation:${NC}"
    echo -e "  Visit: ${BLUE}https://github.com/masterfabric/masterfabric-pre-release${NC}"
    echo ""
    echo -e "${GREEN}Happy security scanning! 🔒${NC}"
else
    echo -e "${RED}Error: Installation verification failed${NC}"
    echo -e "${YELLOW}The binary was copied but is not accessible in PATH${NC}"
    echo -e "${YELLOW}Please check your PATH environment variable${NC}"
    exit 1
fi
