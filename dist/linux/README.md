# MasterFabric Pre-Release Security Checker - Linux Package

## 🚀 Quick Installation

### Option 1: Direct Installation (Recommended)

```bash
# Download and install in one command
curl -fsSL https://raw.githubusercontent.com/masterfabric/masterfabric-pre-release/main/dist/linux/install.sh | bash
```

### Option 2: Manual Installation

1. **Download the package:**
   ```bash
   # Download the latest release
   wget https://github.com/masterfabric/masterfabric-pre-release/releases/latest/download/masterfabric-linux.tar.gz
   
   # Extract the package
   tar -xzf masterfabric-linux.tar.gz
   cd masterfabric-linux
   ```

2. **Run the installation script:**
   ```bash
   ./install.sh
   ```

## 📋 Prerequisites

- **Linux Distribution**: Ubuntu 18.04+, Debian 9+, CentOS 7+, RHEL 7+, Fedora 30+, Arch Linux, openSUSE 15+
- **Administrator privileges** (for installation to `/usr/local/bin`)
- **Internet connection** (for CVE database queries and dependency installation)
- **Package manager**: apt-get, yum, dnf, pacman, or zypper

## 🔧 Usage

After installation, you can use the `masterfabric-prerelease` command from anywhere:

```bash
# Show help
masterfabric-prerelease --help

# Show version
masterfabric-prerelease --version

# Scan a project directory
masterfabric-prerelease --check /path/to/your/project

# Scan with verbose output
masterfabric-prerelease --check /path/to/your/project --verbose

# Scan and export HTML report
masterfabric-prerelease --check /path/to/your/project --html security_report.html

# Scan with both verbose output and HTML export
masterfabric-prerelease --check /path/to/your/project --verbose --html report.html
```

## 📊 Example Output

```bash
$ masterfabric-prerelease --check ./my-project --html report.html

================================================================================
                    MasterFabric Pre-Release Security Checker                   
                    Enterprise-Grade Security Analysis & CVE Detection          
                    Real-time vulnerability scanning with NVD & OSV APIs        
================================================================================

=== CVE API Integration Status ===
API Calls Made: 6
NVD Queries: 3
OSV Queries: 3
Vulnerabilities Found: 25
Data Source: Live APIs (NVD + OSV)

=== Security Analysis Summary ===
Total Findings: 25
Critical: 3
High: 12
Medium: 9
Low: 1

✓ HTML report exported to: report.html
✗ Security policy violations found!
```

## 🗑️ Uninstallation

To remove MasterFabric Security Checker:

```bash
# Download and run uninstall script
curl -fsSL https://raw.githubusercontent.com/masterfabric/masterfabric-pre-release/main/dist/linux/uninstall.sh | bash
```

Or manually:
```bash
sudo rm -f /usr/local/bin/masterfabric-prerelease
```

## 🔍 Troubleshooting

### Permission Issues
If you encounter permission errors:
```bash
# Make sure you have admin privileges
sudo ./install.sh
```

### Binary Not Found
If the command is not found after installation:
```bash
# Check if /usr/local/bin is in your PATH
echo $PATH

# Add to PATH if needed (add to ~/.bashrc or ~/.zshrc)
export PATH="/usr/local/bin:$PATH"
source ~/.bashrc  # or source ~/.zshrc
```

### Missing Dependencies
The installation script automatically detects and installs missing dependencies:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install libcurl4-openssl-dev libjson-c-dev curl
```

**CentOS/RHEL/Fedora:**
```bash
# For CentOS/RHEL
sudo yum install libcurl-devel json-c-devel curl

# For Fedora
sudo dnf install libcurl-devel json-c-devel curl
```

**Arch Linux:**
```bash
sudo pacman -S curl json-c
```

**openSUSE:**
```bash
sudo zypper install libcurl-devel libjson-c-devel curl
```

### Network Issues
If CVE API calls fail:
- Ensure you have internet connectivity
- Check firewall settings
- Verify DNS resolution
- Check if your system can access GitHub and NVD/OSV APIs

### Build from Source
If you prefer to build from source:

```bash
# Install build dependencies
sudo apt-get install build-essential libcurl4-openssl-dev libjson-c-dev pkg-config

# Clone and build
git clone https://github.com/masterfabric/masterfabric-pre-release.git
cd masterfabric-pre-release/checker_tool
make -f Makefile.linux
sudo make -f Makefile.linux install
```

## 📚 Documentation

- **Full Documentation**: [GitHub Repository](https://github.com/masterfabric/masterfabric-pre-release)
- **Security Policy**: [Security Policy Guide](https://github.com/masterfabric/masterfabric-pre-release/docs/security_policy_spec.md)
- **CVE Integration**: [CVE API Documentation](https://github.com/masterfabric/masterfabric-pre-release/docs/real_time_cve_api.md)

## 🆘 Support

- **Issues**: [GitHub Issues](https://github.com/masterfabric/masterfabric-pre-release/issues)
- **Discussions**: [GitHub Discussions](https://github.com/masterfabric/masterfabric-pre-release/discussions)
- **Security**: [Security Policy](https://github.com/masterfabric/masterfabric-pre-release/SECURITY.md)

## 📄 License

This project is licensed under the GNU Affero General Public License v3.0 - see the [LICENSE](https://github.com/masterfabric/masterfabric-pre-release/LICENSE) file for details.

---

**MasterFabric Security Checker v1.0.0** - Enterprise-Grade Security Analysis & CVE Detection
