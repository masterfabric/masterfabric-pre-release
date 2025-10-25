#!/bin/bash

# MasterFabric Pre-Release Security Checker - Linux Uninstallation Script
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

echo -e "${BLUE}================================================================================${NC}"
echo -e "${BLUE}                    MasterFabric Pre-Release Security Checker                   ${NC}"
echo -e "${BLUE}                    Linux Uninstallation Script v1.0.0                         ${NC}"
echo -e "${BLUE}================================================================================${NC}"
echo ""

# Check if binary is installed
if [[ ! -f "$INSTALL_DIR/$BINARY_NAME" ]]; then
    echo -e "${YELLOW}MasterFabric Security Checker is not installed.${NC}"
    echo -e "${YELLOW}Nothing to uninstall.${NC}"
    exit 0
fi

# Show current version
INSTALLED_VERSION=$("$INSTALL_DIR/$BINARY_NAME" --version 2>/dev/null || echo "Unknown")
echo -e "${BLUE}Found installed version: $INSTALLED_VERSION${NC}"
echo ""

# Confirm uninstallation
echo -e "${YELLOW}Are you sure you want to uninstall MasterFabric Security Checker?${NC}"
read -p "Type 'yes' to confirm: " -r
if [[ ! $REPLY =~ ^[Yy][Ee][Ss]$ ]]; then
    echo -e "${BLUE}Uninstallation cancelled.${NC}"
    exit 0
fi

# Remove the binary
echo -e "${BLUE}Removing MasterFabric Security Checker...${NC}"
if sudo rm -f "$INSTALL_DIR/$BINARY_NAME"; then
    echo -e "${GREEN}✓ Binary removed successfully${NC}"
else
    echo -e "${RED}Error: Failed to remove binary from $INSTALL_DIR${NC}"
    exit 1
fi

# Verify removal
echo -e "${BLUE}Verifying removal...${NC}"
if ! command -v "$BINARY_NAME" >/dev/null 2>&1; then
    echo -e "${GREEN}✓ Uninstallation successful!${NC}"
    echo ""
    echo -e "${BLUE}================================================================================${NC}"
    echo -e "${GREEN}🎉 MasterFabric Security Checker has been uninstalled successfully! 🎉${NC}"
    echo -e "${BLUE}================================================================================${NC}"
    echo ""
    echo -e "${YELLOW}Thank you for using MasterFabric Security Checker!${NC}"
    echo -e "${BLUE}Visit: https://github.com/masterfabric/masterfabric-pre-release${NC}"
else
    echo -e "${RED}Error: Uninstallation verification failed${NC}"
    echo -e "${YELLOW}The binary may still be accessible in PATH${NC}"
    exit 1
fi
