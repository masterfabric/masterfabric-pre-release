# Vulnerable Go Application

This is a deliberately vulnerable Go application designed to demonstrate various security vulnerabilities for testing the MasterFabric Security Checker.

## ⚠️ WARNING

**DO NOT USE THIS APPLICATION IN PRODUCTION!**

This application contains intentional security vulnerabilities for educational and testing purposes only.

## 🚨 Vulnerabilities Included

### 1. **Dependency Vulnerabilities**
- Outdated Gin framework (v1.6.3) with known CVEs
- Outdated JWT library (v3.2.2) with security issues
- Outdated SQLite driver (v1.14.6) with vulnerabilities
- Multiple other outdated Go modules with known security issues
- Outdated crypto libraries and dependencies

### 2. **Authentication & Authorization**
- Hardcoded secret keys and passwords
- Weak password hashing (MD5)
- No rate limiting on login attempts
- No account lockout mechanisms
- Missing authentication checks on protected endpoints
- Insecure JWT token generation with weak secrets

### 3. **Cryptographic Issues**
- Weak encryption implementation (XOR with weak key)
- Insecure random number generation
- Predictable session IDs and tokens
- Hardcoded encryption keys
- Using MD5 for password hashing

### 4. **Injection Vulnerabilities**
- SQL Injection in user queries
- Command Injection in system commands
- Path Traversal in file operations
- No input validation or sanitization

### 5. **Information Disclosure**
- Detailed error messages exposing internal information
- Hardcoded credentials in source code
- Sensitive data in error responses
- Debug mode enabled
- Environment variables exposed
- Process information disclosed

### 6. **Input Validation**
- No input sanitization
- No parameter validation
- Direct string concatenation in queries
- No CSRF protection

### 7. **Network Security**
- Running on all interfaces (0.0.0.0)
- No HTTPS enforcement
- No security headers
- Debug mode in production

## 🛠 Setup Instructions

1. **Install Go dependencies:**
   ```bash
   go mod tidy
   ```

2. **Run the application:**
   ```bash
   go run main.go
   ```

3. **Access the application:**
   - Open your browser to `http://localhost:8080`
   - The application will be running in debug mode

## 🧪 Testing with MasterFabric Security Checker

Run the security checker on this application:

```bash
# From the project root
cd cases/demo_go_vulnerable_app
../../checker_tool/bin/masterfabric-prerelease --check . --verbose

# Generate HTML report
../../checker_tool/bin/masterfabric-prerelease --check . --html go_security_report.html
```

## 📋 Expected Security Findings

The MasterFabric Security Checker should detect:

- **Critical**: Outdated dependencies with known CVEs
- **High**: Hardcoded secrets and credentials
- **High**: SQL injection vulnerabilities
- **High**: Command injection vulnerabilities
- **Medium**: Weak cryptographic implementations
- **Medium**: Information disclosure issues
- **Low**: Missing input validation
- **Low**: No authentication on protected endpoints

## 🔍 Manual Testing

You can manually test the vulnerabilities:

### SQL Injection
```bash
curl -X GET "http://localhost:8080/users/1 OR 1=1"
```

### Command Injection
```bash
curl -X POST "http://localhost:8080/execute" \
  -H "Content-Type: application/json" \
  -d '{"command": "ls -la; cat /etc/passwd"}'
```

### Path Traversal
```bash
curl -X POST "http://localhost:8080/read" \
  -H "Content-Type: application/json" \
  -d '{"filename": "../../../etc/passwd"}'
```

### Weak Authentication
```bash
curl -X POST "http://localhost:8080/login" \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "hello"}'
```

### Information Disclosure
```bash
curl -X GET "http://localhost:8080/debug"
```

### Admin Panel Access (No Auth)
```bash
curl -X GET "http://localhost:8080/admin"
```

## 📚 Learning Objectives

This demo application helps you understand:

- Common Go security vulnerabilities
- How to identify vulnerable dependencies in go.mod
- The importance of input validation in Go
- Secure coding practices for Go applications
- How security scanners detect issues in Go code

## 🔒 Security Best Practices (What NOT to do)

This application demonstrates what NOT to do:

- ❌ Don't use hardcoded secrets
- ❌ Don't use weak hashing algorithms (MD5)
- ❌ Don't concatenate user input into SQL queries
- ❌ Don't execute shell commands with user input
- ❌ Don't expose detailed error information
- ❌ Don't run in debug mode in production
- ❌ Don't use weak encryption algorithms
- ❌ Don't skip input validation

## 🏗 Architecture

The application uses:
- **Gin** web framework
- **SQLite** database
- **JWT** for authentication (insecurely implemented)
- **HTML templates** for rendering

## 📄 License

This demo application is part of the MasterFabric Pre-Release Security Checker project and is licensed under the GNU Affero General Public License v3.0.
