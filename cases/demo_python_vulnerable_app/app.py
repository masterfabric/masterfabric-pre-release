#!/usr/bin/env python3
"""
Vulnerable Python Flask Application for Security Testing
This application contains intentional security vulnerabilities for demonstration purposes.
DO NOT USE IN PRODUCTION!
"""

import os
import hashlib
import base64
import random
import sqlite3
import pickle
import yaml
import subprocess
from flask import Flask, request, render_template_string, jsonify, session, redirect, url_for
from werkzeug.security import generate_password_hash, check_password_hash
import requests
from cryptography.fernet import Fernet
import json

app = Flask(__name__)

# VULNERABILITY: Hardcoded secret key
app.secret_key = 'super-secret-key-12345'

# VULNERABILITY: Hardcoded database credentials
DATABASE_URL = "sqlite:///vulnerable_app.db"
DB_USERNAME = "admin"
DB_PASSWORD = "password123"

# VULNERABILITY: Hardcoded API keys and secrets
API_KEY = "sk-1234567890abcdef"
JWT_SECRET = "jwt-secret-key-very-insecure"
ENCRYPTION_KEY = "my-very-weak-encryption-key"

# VULNERABILITY: Weak cryptographic implementation
def weak_hash_password(password):
    """VULNERABILITY: Using MD5 for password hashing (very weak)"""
    return hashlib.md5(password.encode()).hexdigest()

def weak_encrypt(data):
    """VULNERABILITY: Using weak encryption"""
    # Using simple XOR with a weak key
    key = "weakkey"
    encrypted = ""
    for i, char in enumerate(data):
        encrypted += chr(ord(char) ^ ord(key[i % len(key)]))
    return base64.b64encode(encrypted.encode()).decode()

def weak_decrypt(encrypted_data):
    """VULNERABILITY: Weak decryption"""
    data = base64.b64decode(encrypted_data.encode()).decode()
    key = "weakkey"
    decrypted = ""
    for i, char in enumerate(data):
        decrypted += chr(ord(char) ^ ord(key[i % len(key)]))
    return decrypted

# VULNERABILITY: Insecure random number generation
def generate_session_id():
    """VULNERABILITY: Using predictable random number generation"""
    return str(random.randint(1000, 9999))

def generate_csrf_token():
    """VULNERABILITY: Predictable CSRF token generation"""
    return hashlib.sha1(str(random.randint(1, 100)).encode()).hexdigest()

# VULNERABILITY: SQL Injection vulnerable function
def get_user_by_id(user_id):
    """VULNERABILITY: SQL Injection - direct string concatenation"""
    conn = sqlite3.connect('vulnerable_app.db')
    cursor = conn.cursor()
    # VULNERABILITY: Direct string concatenation without parameterization
    query = f"SELECT * FROM users WHERE id = {user_id}"
    cursor.execute(query)
    result = cursor.fetchone()
    conn.close()
    return result

def create_user(username, password, email):
    """VULNERABILITY: SQL Injection in user creation"""
    conn = sqlite3.connect('vulnerable_app.db')
    cursor = conn.cursor()
    # VULNERABILITY: Direct string concatenation
    query = f"INSERT INTO users (username, password, email) VALUES ('{username}', '{password}', '{email}')"
    cursor.execute(query)
    conn.commit()
    conn.close()

# VULNERABILITY: Command Injection
def execute_system_command(command):
    """VULNERABILITY: Command injection - using shell=True"""
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result.stdout

def ping_host(host):
    """VULNERABILITY: Command injection in ping function"""
    # VULNERABILITY: Direct command execution without sanitization
    command = f"ping -c 1 {host}"
    return execute_system_command(command)

# VULNERABILITY: Path Traversal
def read_file(filename):
    """VULNERABILITY: Path traversal - no path validation"""
    try:
        with open(filename, 'r') as f:
            return f.read()
    except Exception as e:
        return f"Error reading file: {str(e)}"

# VULNERABILITY: Insecure Deserialization
def deserialize_data(data):
    """VULNERABILITY: Insecure pickle deserialization"""
    try:
        return pickle.loads(base64.b64decode(data))
    except Exception as e:
        return f"Deserialization error: {str(e)}"

def deserialize_yaml(data):
    """VULNERABILITY: Insecure YAML deserialization"""
    try:
        return yaml.load(data, Loader=yaml.Loader)  # VULNERABILITY: Using unsafe loader
    except Exception as e:
        return f"YAML deserialization error: {str(e)}"

# VULNERABILITY: Insecure HTTP requests
def make_insecure_request(url):
    """VULNERABILITY: Making requests without SSL verification"""
    try:
        # VULNERABILITY: Disabled SSL verification
        response = requests.get(url, verify=False, timeout=5)
        return response.text
    except Exception as e:
        return f"Request error: {str(e)}"

# VULNERABILITY: Information disclosure
@app.route('/')
def index():
    """VULNERABILITY: Information disclosure in error messages"""
    try:
        # VULNERABILITY: Exposing internal information
        return f"""
        <h1>Vulnerable Python App</h1>
        <p>Database URL: {DATABASE_URL}</p>
        <p>API Key: {API_KEY}</p>
        <p>JWT Secret: {JWT_SECRET}</p>
        <p>Encryption Key: {ENCRYPTION_KEY}</p>
        <p>Session ID: {generate_session_id()}</p>
        <p>CSRF Token: {generate_csrf_token()}</p>
        <a href="/login">Login</a> | <a href="/register">Register</a>
        """
    except Exception as e:
        # VULNERABILITY: Detailed error information exposure
        return f"Error: {str(e)}<br>Type: {type(e).__name__}<br>File: {__file__}"

@app.route('/login', methods=['GET', 'POST'])
def login():
    """VULNERABILITY: Weak authentication"""
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        
        # VULNERABILITY: Weak password hashing
        hashed_password = weak_hash_password(password)
        
        # VULNERABILITY: No rate limiting, no account lockout
        user = get_user_by_id(1)  # Always get user with ID 1
        
        if user and user[2] == hashed_password:  # Assuming password is in index 2
            session['user_id'] = user[0]
            session['username'] = username
            return redirect(url_for('dashboard'))
        else:
            # VULNERABILITY: Information disclosure in error messages
            return f"Login failed for user: {username}<br>Attempted password hash: {hashed_password}"
    
    return """
    <form method="post">
        <input type="text" name="username" placeholder="Username" required>
        <input type="password" name="password" placeholder="Password" required>
        <button type="submit">Login</button>
    </form>
    """

@app.route('/register', methods=['GET', 'POST'])
def register():
    """VULNERABILITY: Insecure user registration"""
    if request.method == 'POST':
        username = request.form.get('username')
        password = request.form.get('password')
        email = request.form.get('email')
        
        # VULNERABILITY: No input validation
        # VULNERABILITY: Weak password hashing
        hashed_password = weak_hash_password(password)
        
        try:
            create_user(username, hashed_password, email)
            return f"User {username} created successfully with password hash: {hashed_password}"
        except Exception as e:
            # VULNERABILITY: Detailed error exposure
            return f"Registration failed: {str(e)}<br>SQL Query would be: INSERT INTO users (username, password, email) VALUES ('{username}', '{hashed_password}', '{email}')"
    
    return """
    <form method="post">
        <input type="text" name="username" placeholder="Username" required>
        <input type="password" name="password" placeholder="Password" required>
        <input type="email" name="email" placeholder="Email" required>
        <button type="submit">Register</button>
    </form>
    """

@app.route('/dashboard')
def dashboard():
    """VULNERABILITY: No authentication check"""
    if 'user_id' not in session:
        return redirect(url_for('login'))
    
    # VULNERABILITY: Information disclosure
    return f"""
    <h1>Dashboard</h1>
    <p>Welcome, {session.get('username', 'Unknown')}!</p>
    <p>User ID: {session.get('user_id')}</p>
    <p>Session Data: {dict(session)}</p>
    <a href="/encrypt">Encrypt Data</a> | <a href="/decrypt">Decrypt Data</a>
    <a href="/ping">Ping Host</a> | <a href="/read">Read File</a>
    """

@app.route('/encrypt', methods=['GET', 'POST'])
def encrypt_data():
    """VULNERABILITY: Weak encryption endpoint"""
    if request.method == 'POST':
        data = request.form.get('data')
        encrypted = weak_encrypt(data)
        return f"Encrypted data: {encrypted}"
    
    return """
    <form method="post">
        <textarea name="data" placeholder="Data to encrypt" required></textarea>
        <button type="submit">Encrypt</button>
    </form>
    """

@app.route('/decrypt', methods=['GET', 'POST'])
def decrypt_data():
    """VULNERABILITY: Weak decryption endpoint"""
    if request.method == 'POST':
        encrypted_data = request.form.get('encrypted_data')
        decrypted = weak_decrypt(encrypted_data)
        return f"Decrypted data: {decrypted}"
    
    return """
    <form method="post">
        <textarea name="encrypted_data" placeholder="Encrypted data" required></textarea>
        <button type="submit">Decrypt</button>
    </form>
    """

@app.route('/ping', methods=['GET', 'POST'])
def ping_endpoint():
    """VULNERABILITY: Command injection endpoint"""
    if request.method == 'POST':
        host = request.form.get('host')
        result = ping_host(host)
        return f"<pre>Ping result for {host}:\n{result}</pre>"
    
    return """
    <form method="post">
        <input type="text" name="host" placeholder="Host to ping" required>
        <button type="submit">Ping</button>
    </form>
    """

@app.route('/read', methods=['GET', 'POST'])
def read_file_endpoint():
    """VULNERABILITY: Path traversal endpoint"""
    if request.method == 'POST':
        filename = request.form.get('filename')
        content = read_file(filename)
        return f"<pre>File content of {filename}:\n{content}</pre>"
    
    return """
    <form method="post">
        <input type="text" name="filename" placeholder="Filename to read" required>
        <button type="submit">Read File</button>
    </form>
    """

@app.route('/deserialize', methods=['GET', 'POST'])
def deserialize_endpoint():
    """VULNERABILITY: Insecure deserialization endpoint"""
    if request.method == 'POST':
        data = request.form.get('data')
        deserialized = deserialize_data(data)
        return f"Deserialized data: {deserialized}"
    
    return """
    <form method="post">
        <textarea name="data" placeholder="Base64 encoded pickle data" required></textarea>
        <button type="submit">Deserialize</button>
    </form>
    """

@app.route('/yaml', methods=['GET', 'POST'])
def yaml_endpoint():
    """VULNERABILITY: Insecure YAML deserialization"""
    if request.method == 'POST':
        yaml_data = request.form.get('yaml_data')
        parsed = deserialize_yaml(yaml_data)
        return f"Parsed YAML: {parsed}"
    
    return """
    <form method="post">
        <textarea name="yaml_data" placeholder="YAML data" required></textarea>
        <button type="submit">Parse YAML</button>
    </form>
    """

@app.route('/request', methods=['GET', 'POST'])
def request_endpoint():
    """VULNERABILITY: Insecure HTTP requests"""
    if request.method == 'POST':
        url = request.form.get('url')
        result = make_insecure_request(url)
        return f"<pre>Response from {url}:\n{result}</pre>"
    
    return """
    <form method="post">
        <input type="url" name="url" placeholder="URL to request" required>
        <button type="submit">Make Request</button>
    </form>
    """

if __name__ == '__main__':
    # VULNERABILITY: Running in debug mode in production
    app.run(debug=True, host='0.0.0.0', port=5000)
