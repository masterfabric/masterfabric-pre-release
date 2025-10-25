# Vulnerable Python Flask Application

This is a deliberately vulnerable Python Flask application designed to demonstrate various security vulnerabilities for testing the MasterFabric Security Checker.

## ⚠️ WARNING

**DO NOT USE THIS APPLICATION IN PRODUCTION!**

This application contains intentional security vulnerabilities for educational and testing purposes only.

## 🚨 Vulnerabilities Included

### 1. **Dependency Vulnerabilities**
- Outdated Flask (1.1.1) with known CVEs
- Outdated requests (2.20.0) with security issues
- Outdated cryptography (2.8) library
- Outdated PyYAML (5.1.2) with deserialization vulnerabilities
- Multiple other outdated packages with known security issues

### 2. **Authentication & Authorization**
- Hardcoded secret keys and passwords
- Weak password hashing (MD5)
- No rate limiting on login attempts
- No account lockout mechanisms
- Missing authentication checks on protected endpoints

### 3. **Cryptographic Issues**
- Weak encryption implementation (XOR with weak key)
- Insecure random number generation
- Predictable session IDs and CSRF tokens
- Hardcoded encryption keys

### 4. **Injection Vulnerabilities**
- SQL Injection in user queries
- Command Injection in system commands
- Path Traversal in file operations
- Template Injection potential

### 5. **Insecure Deserialization**
- Unsafe pickle deserialization
- Insecure YAML deserialization with unsafe loader

### 6. **Information Disclosure**
- Detailed error messages exposing internal information
- Hardcoded credentials in source code
- Sensitive data in error responses
- Debug mode enabled

### 7. **Network Security**
- HTTP requests without SSL verification
- Insecure HTTP endpoints
- No input validation on network requests

### 8. **Input Validation**
- No input sanitization
- No parameter validation
- Direct string concatenation in queries

## 🛠 Setup Instructions

1. **Install Python dependencies:**
   ```bash
   pip install -r requirements.txt
   ```

2. **Run the application:**
   ```bash
   python app.py
   ```

3. **Access the application:**
   - Open your browser to `http://localhost:5000`
   - The application will be running in debug mode

## 🧪 Testing with MasterFabric Security Checker

Run the security checker on this application:

```bash
# From the project root
cd cases/demo_python_vulnerable_app
../../checker_tool/bin/masterfabric-prerelease --check . --verbose

# Generate HTML report
../../checker_tool/bin/masterfabric-prerelease --check . --html python_security_report.html
```

## 📋 Expected Security Findings

The MasterFabric Security Checker should detect:

- **Critical**: Outdated dependencies with known CVEs
- **High**: Hardcoded secrets and credentials
- **High**: SQL injection vulnerabilities
- **High**: Command injection vulnerabilities
- **Medium**: Weak cryptographic implementations
- **Medium**: Insecure deserialization
- **Low**: Information disclosure issues
- **Low**: Missing input validation

## 🔍 Manual Testing

You can manually test the vulnerabilities:

1. **SQL Injection**: Try `1 OR 1=1` in user ID fields
2. **Command Injection**: Try `localhost; cat /etc/passwd` in ping field
3. **Path Traversal**: Try `../../../etc/passwd` in file read field
4. **Deserialization**: Try base64 encoded pickle payloads
5. **YAML Injection**: Try YAML with `!!python/object` tags

## 📚 Learning Objectives

This demo application helps you understand:

- Common Python security vulnerabilities
- How to identify vulnerable dependencies
- The importance of input validation
- Secure coding practices
- How security scanners detect issues

## 🔒 Security Best Practices (What NOT to do)

This application demonstrates what NOT to do:

- ❌ Don't use hardcoded secrets
- ❌ Don't use weak hashing algorithms
- ❌ Don't concatenate user input into queries
- ❌ Don't disable SSL verification
- ❌ Don't use unsafe deserialization
- ❌ Don't expose detailed error information
- ❌ Don't run in debug mode in production

## 📄 License

This demo application is part of the MasterFabric Pre-Release Security Checker project and is licensed under the GNU Affero General Public License v3.0.
