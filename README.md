# MasterFabric Pre-Release Security Checker

A comprehensive C-based security analysis tool designed to perform automated security checks on C/C++, JavaScript/TypeScript, Python, Rust, Go, and Dart projects before release. This tool acts as a final control layer to prevent the deployment of code with known vulnerabilities, weak cryptography, or insufficient anti-reverse engineering protections.

## 🚀 Features

- **Multi-Language Support**: Analyzes C/C++, JavaScript/TypeScript, Python, Rust, Go, and Dart projects
- **Real-Time CVE API Integration**: Live vulnerability data from NVD and OSV APIs
- **Dependency Analysis**: Scans package managers for vulnerable dependencies and CVEs
- **Cryptographic Scanner**: Detects weak ciphers, hardcoded secrets, and insecure random generation
- **Binary Hardening**: Verifies compiler flags and binary security features
- **Static Analysis**: Integrates with clang-tidy and cppcheck for memory safety issues
- **Policy-Driven**: Configurable security policies via `.security.yaml` files
- **CI/CD Ready**: Designed for integration into build pipelines

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Configuration](#configuration)
- [Security Checks](#security-checks)
- [Demo Application](#demo-application)
- [API Reference](#api-reference)
- [Contributing](#contributing)
- [License](#license)

## 🛠 Installation

### 🚀 Quick Install (macOS)

**One-line installation:**
```bash
curl -fsSL https://raw.githubusercontent.com/masterfabric/masterfabric-pre-release/main/dist/macos/install.sh | bash
```

**Or download and install manually:**
```bash
# Download the latest release
wget https://github.com/masterfabric/masterfabric-pre-release/releases/latest/download/masterfabric-macos-v1.0.0.tar.gz

# Extract and install
tar -xzf masterfabric-macos-v1.0.0.tar.gz
cd masterfabric-macos
./install.sh
```

### Prerequisites

- **macOS 10.15+** (Catalina or later)
- **Administrator privileges** (for installation to `/usr/local/bin`)
- **Internet connection** (for CVE database queries)

### Build from Source

```bash
# Clone the repository
git clone https://github.com/masterfabric/masterfabric-pre-release.git
cd masterfabric-pre-release

# Install dependencies (macOS)
brew install curl json-c pkg-config

# Build the security checker
cd checker_tool
make

# Install system-wide (optional)
sudo make install
```

### Verify Installation

```bash
masterfabric-prerelease --version
```

### Uninstallation

To remove MasterFabric Security Checker:
```bash
curl -fsSL https://raw.githubusercontent.com/masterfabric/masterfabric-pre-release/main/dist/macos/uninstall.sh | bash
```

## 🚀 Quick Start

### 1. Create a Security Policy

Create a `.security.yaml` file in your project root:

```yaml
failure_threshold: HIGH

approved_sdks:
  - name: "react"
    versions: ">=18.0.0"
  - name: "express"
    versions: ">=4.18.0"

blacklisted_versions:
  - "lodash@<4.17.21"
  - "node-fetch@<2.6.7"

minimum_acceptable_standards:
  symmetric_encryption: "AES-256-GCM"
  hashing: "SHA3-256"

hardcoded_secret_scan:
  enabled: true
  entropy_threshold: 4.5

memory_safety_checks: true
concurrency_checks: true
```

### 2. Run Security Check

```bash
# Check current directory
masterfabric-prerelease --check

# Check specific directory
masterfabric-prerelease --check /path/to/project

# Verbose output
masterfabric-prerelease --check --verbose

# Export results to HTML report
masterfabric-prerelease --check /path/to/project --html security_report.html

# Verbose output with HTML export
masterfabric-prerelease --check /path/to/project --verbose --html report.html
```

### 3. Review Results

The tool will output a comprehensive security report with:
- Summary of findings by severity
- Detailed descriptions of each issue
- Specific recommendations for remediation
- Policy compliance status
- **HTML Export**: Professional reports with black and white design for sharing and printing

## 🔄 Real-Time CVE API Integration

The MasterFabric Security Checker now includes **real-time CVE API integration** that fetches live vulnerability data from official sources:

### Supported CVE Sources
- **NVD API**: National Vulnerability Database (NIST)
- **OSV API**: Open Source Vulnerabilities database

### Real-Time Analysis Features
- **Live vulnerability data** from official CVE databases
- **Multi-platform package analysis** (Flutter, Node.js, Python, Rust, Go)
- **CVSS score calculation** and severity classification
- **Version-specific vulnerability matching**

### Example Output
```bash
Starting real-time CVE analysis using NVD and OSV APIs...
Checking Flutter dependencies for CVEs...
Fetching CVEs from NVD for package: http
Fetching CVEs from OSV for package: http (version: 0.12.2)
Found 3 vulnerabilities for package http
Found 2 CVE vulnerabilities in Flutter dependencies via API
```

## ⚙️ Configuration

### Security Policy File

The `.security.yaml` file defines your security requirements. See the [Security Policy Specification](docs/security_policy_spec.md) for complete documentation.

### Key Configuration Options

- **`failure_threshold`**: Minimum severity to fail the build
- **`approved_sdks`**: Whitelist of allowed dependencies
- **`blacklisted_versions`**: Explicitly forbidden versions
- **`minimum_acceptable_standards`**: Cryptographic requirements
- **`hardcoded_secret_scan`**: Secret detection settings
- **`required_hardening_flags`**: Compiler security flags
- **`binary_checks`**: Binary security feature requirements

## 🔍 Security Checks

### 1. Dependency Analysis
- Scans `package.json`, `requirements.txt`, `Cargo.toml`, `go.mod`
- Checks against approved SDK whitelist
- Identifies blacklisted versions
- Detects known CVEs in dependencies

### 2. Cryptographic Scanner
- Detects weak algorithms (DES, MD5, SHA1, RC4)
- Identifies hardcoded secrets using entropy analysis
- Checks for insecure random number generation
- Validates minimum cryptographic standards

### 3. Binary Hardening
- Verifies compiler security flags in build files
- Checks for stack protection, ASLR/PIE, and RELRO
- Validates debug symbol stripping
- Analyzes ELF binary security features

### 4. Static Analysis
- Integrates with clang-tidy for C/C++ analysis
- Uses cppcheck for additional security checks
- Focuses on memory safety and concurrency issues
- Configurable check levels

## 🎯 Demo Applications

Two vulnerable applications are included to demonstrate the tool's capabilities:

### Next.js Demo Application
```bash
# Run the Next.js demo
cd cases/demo_vulnerable_app
npm install
npm run dev

# Test with security checker
cd ../../checker_tool
make test
```

### Flutter Demo Application
```bash
# Run the Flutter demo
cd cases/demo_flutter_vulnerable_app
flutter pub get
flutter run

# Test with security checker
cd ../../checker_tool
make test
```

Both demo applications contain intentional vulnerabilities including:
- Outdated dependencies with known CVEs
- Weak cryptographic implementations
- Hardcoded secrets and API keys
- Insecure random number generation
- SQL injection vulnerabilities
- Insecure data storage practices

## 📚 API Reference

### Command Line Interface

```bash
masterfabric-prerelease [OPTIONS]

Options:
  --check [PATH]    Check directory for security issues
  --html [FILE]     Export results to HTML file
  --verbose         Enable verbose output
  --help            Show help message
  --version         Show version information
```

### Exit Codes

- `0`: All checks passed
- `1`: Policy violations found
- `2`: Configuration error
- `3`: Scan error

### Integration Examples

#### GitHub Actions

```yaml
name: Security Check
on: [push, pull_request]

jobs:
  security:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Build Security Checker
        run: |
          cd checker_tool
          make
      - name: Run Security Check
        run: |
          ./checker_tool/bin/masterfabric-prerelease --check . --verbose
```

#### Makefile Integration

```makefile
.PHONY: security-check
security-check:
	@echo "Running security analysis..."
	@./checker_tool/bin/masterfabric-prerelease --check . --verbose
	@echo "Security check completed"

build: security-check
	# Your build process here
```

## 🏗 Architecture

### Project Structure

```
masterfabric-pre-release/
├── checker_tool/              # C-based security checker
│   ├── src/                  # Source files
│   ├── include/              # Header files
│   ├── Makefile             # Build configuration
│   └── README.md
├── cases/                    # Demo applications
│   ├── demo_vulnerable_app/  # Next.js demo with vulnerabilities
│   └── demo_flutter_vulnerable_app/  # Flutter demo with vulnerabilities
├── docs/                     # Documentation
└── README.md                 # This file
```

### Core Components

- **CLI Framework**: Argument parsing and path resolution
- **YAML Parser**: Custom parser for security policy files
- **SDK Analyzer**: Dependency vulnerability scanning
- **Crypto Scanner**: Cryptographic weakness detection
- **Binary Parser**: Binary hardening verification
- **Static Analyzer**: Integration with clang-tidy/cppcheck
- **Report Generator**: Results aggregation and formatting

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Clone and build
git clone <repository-url>
cd masterfabric-pre-release/checker_tool
make debug

# Run tests
make test

# Check dependencies
make check-deps
```

### Adding New Checks

1. Create new analyzer module in `src/`
2. Add header file in `include/`
3. Update main.c to call new analyzer
4. Add configuration options to YAML parser
5. Update documentation

## 📄 License

This project is licensed under the GNU Affero General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## 🆘 Support

- **Documentation**: [Security Policy Spec](docs/security_policy_spec.md)
- **Issues**: [GitHub Issues](https://github.com/masterfabric/masterfabric-pre-release/issues)
- **Discussions**: [GitHub Discussions](https://github.com/masterfabric/masterfabric-pre-release/discussions)

## 🙏 Acknowledgments

- clang-tidy and cppcheck for static analysis capabilities
- The security research community for vulnerability databases
- Contributors and users who help improve the tool

---

**⚠️ Important**: This tool is designed to catch common security issues but should not be your only security measure. Always follow security best practices and conduct regular security audits.
