use actix_web::{web, App, HttpServer, HttpResponse, Result, middleware::Logger};
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use std::fs;
use std::process::Command;
use std::path::Path;
use std::env;
use std::process;
use md5;
use sha1::Sha1;
use sha2::{Sha256, Digest};
use base64;
use hex;
use serde_json::json;
use std::io::Read;

// VULNERABILITY: Hardcoded secrets and credentials
const SECRET_KEY: &str = "super-secret-key-12345";
const JWT_SECRET: &str = "jwt-secret-key-very-insecure";
const DB_PASSWORD: &str = "password123";
const API_KEY: &str = "sk-1234567890abcdef";
const ENCRYPTION_KEY: &str = "my-very-weak-encryption-key";

// VULNERABILITY: Global state with sensitive data
static mut GLOBAL_STATE: Option<HashMap<String, String>> = None;

#[derive(Serialize, Deserialize)]
struct User {
    id: u32,
    username: String,
    password: String,
    email: String,
}

#[derive(Serialize, Deserialize)]
struct LoginRequest {
    username: String,
    password: String,
}

#[derive(Serialize, Deserialize)]
struct CommandRequest {
    command: String,
}

#[derive(Serialize, Deserialize)]
struct FileRequest {
    filename: String,
}

#[derive(Serialize, Deserialize)]
struct EncryptRequest {
    data: String,
}

#[derive(Serialize, Deserialize)]
struct DecryptRequest {
    encrypted_data: String,
}

// VULNERABILITY: Weak password hashing using MD5
fn weak_hash_password(password: &str) -> String {
    let digest = md5::compute(password.as_bytes());
    format!("{:x}", digest)
}

// VULNERABILITY: Insecure random number generation
fn generate_weak_token() -> String {
    // Using time-based seed makes it predictable
    let timestamp = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_secs();
    format!("{}", timestamp % 10000)
}

// VULNERABILITY: Weak encryption using simple XOR
fn weak_encrypt(data: &str) -> String {
    let key = "weakkey";
    let mut encrypted = Vec::new();
    for (i, byte) in data.bytes().enumerate() {
        encrypted.push(byte ^ key.as_bytes()[i % key.len()]);
    }
    base64::encode(encrypted)
}

// VULNERABILITY: Weak decryption
fn weak_decrypt(encrypted_data: &str) -> Result<String, Box<dyn std::error::Error>> {
    let data = base64::decode(encrypted_data)?;
    let key = "weakkey";
    let mut decrypted = Vec::new();
    for (i, &byte) in data.iter().enumerate() {
        decrypted.push(byte ^ key.as_bytes()[i % key.len()]);
    }
    Ok(String::from_utf8(decrypted)?)
}

// VULNERABILITY: SQL Injection vulnerable function (simulated)
fn get_user_by_id(user_id: &str) -> Result<User, Box<dyn std::error::Error>> {
    // VULNERABILITY: Direct string concatenation without parameterization
    let query = format!("SELECT id, username, password, email FROM users WHERE id = {}", user_id);
    
    // Simulate database query result
    Ok(User {
        id: user_id.parse().unwrap_or(1),
        username: "admin".to_string(),
        password: "5d41402abc4b2a76b9719d911017c592".to_string(), // MD5 of "hello"
        email: "admin@example.com".to_string(),
    })
}

// VULNERABILITY: Command injection
fn execute_command(command: &str) -> Result<String, Box<dyn std::error::Error>> {
    // VULNERABILITY: Using shell execution without sanitization
    let output = if cfg!(target_os = "windows") {
        Command::new("cmd")
            .args(&["/C", command])
            .output()?
    } else {
        Command::new("sh")
            .arg("-c")
            .arg(command)
            .output()?
    };
    
    Ok(String::from_utf8(output.stdout)?)
}

// VULNERABILITY: Path traversal
fn read_file(filename: &str) -> Result<String, Box<dyn std::error::Error>> {
    // VULNERABILITY: No path validation
    let mut file = std::fs::File::open(filename)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents)
}

// VULNERABILITY: Information disclosure in error handling
fn handle_error(error: &dyn std::error::Error, message: &str) -> HttpResponse {
    HttpResponse::InternalServerError().json(json!({
        "error": message,
        "details": error.to_string(),
        "type": std::any::type_name::<dyn std::error::Error>(),
        "file": "main.rs",
        "line": "error_handling"
    }))
}

// VULNERABILITY: Information disclosure endpoint
async fn index() -> Result<HttpResponse> {
    let html = format!(r#"
        <!DOCTYPE html>
        <html>
        <head>
            <title>Vulnerable Rust App</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 40px; }}
                .vulnerable {{ background-color: #ffebee; padding: 20px; border-radius: 5px; margin: 10px 0; }}
                .secret {{ background-color: #fff3e0; padding: 10px; border-radius: 3px; margin: 5px 0; }}
                pre {{ background-color: #f5f5f5; padding: 10px; border-radius: 3px; }}
            </style>
        </head>
        <body>
            <h1>Vulnerable Rust App</h1>
            
            <div class="vulnerable">
                <h2>⚠️ VULNERABLE APPLICATION - DO NOT USE IN PRODUCTION!</h2>
                <p>This application contains intentional security vulnerabilities for testing purposes.</p>
            </div>

            <h2>Exposed Secrets (VULNERABILITY: Information Disclosure)</h2>
            <div class="secret">
                <strong>Secret Key:</strong> {}
            </div>
            <div class="secret">
                <strong>JWT Secret:</strong> {}
            </div>
            <div class="secret">
                <strong>API Key:</strong> {}
            </div>
            <div class="secret">
                <strong>DB Password:</strong> {}
            </div>
            <div class="secret">
                <strong>Session ID:</strong> {}
            </div>

            <h2>Available Endpoints</h2>
            <ul>
                <li><strong>GET /users/{{id}}</strong> - Get user by ID (SQL Injection vulnerable)</li>
                <li><strong>POST /login</strong> - Login (Weak authentication)</li>
                <li><strong>POST /execute</strong> - Execute command (Command Injection)</li>
                <li><strong>POST /read</strong> - Read file (Path Traversal)</li>
                <li><strong>POST /encrypt</strong> - Encrypt data (Weak encryption)</li>
                <li><strong>POST /decrypt</strong> - Decrypt data (Weak decryption)</li>
                <li><strong>GET /debug</strong> - Debug information (Information disclosure)</li>
                <li><strong>GET /admin</strong> - Admin panel (No authentication)</li>
            </ul>

            <h2>Test Examples</h2>
            
            <h3>SQL Injection Test</h3>
            <pre>
curl -X GET "http://localhost:8080/users/1 OR 1=1"
            </pre>

            <h3>Command Injection Test</h3>
            <pre>
curl -X POST "http://localhost:8080/execute" \
  -H "Content-Type: application/json" \
  -d '{{"command": "ls -la; cat /etc/passwd"}}'
            </pre>

            <h3>Path Traversal Test</h3>
            <pre>
curl -X POST "http://localhost:8080/read" \
  -H "Content-Type: application/json" \
  -d '{{"filename": "../../../etc/passwd"}}'
            </pre>

            <h3>Weak Authentication Test</h3>
            <pre>
curl -X POST "http://localhost:8080/login" \
  -H "Content-Type: application/json" \
  -d '{{"username": "admin", "password": "hello"}}'
            </pre>

            <h3>Information Disclosure Test</h3>
            <pre>
curl -X GET "http://localhost:8080/debug"
            </pre>

            <h3>Admin Panel Access (No Auth)</h3>
            <pre>
curl -X GET "http://localhost:8080/admin"
            </pre>

            <h2>Security Vulnerabilities Included</h2>
            <ul>
                <li><strong>Critical:</strong> Outdated dependencies with known CVEs</li>
                <li><strong>High:</strong> Hardcoded secrets and credentials</li>
                <li><strong>High:</strong> SQL injection vulnerabilities</li>
                <li><strong>High:</strong> Command injection vulnerabilities</li>
                <li><strong>Medium:</strong> Weak cryptographic implementations</li>
                <li><strong>Medium:</strong> Information disclosure</li>
                <li><strong>Low:</strong> Missing input validation</li>
                <li><strong>Low:</strong> No authentication on protected endpoints</li>
            </ul>

            <div class="vulnerable">
                <h3>⚠️ REMINDER</h3>
                <p>This application is designed for security testing only. Never use this code in production!</p>
            </div>
        </body>
        </html>
    "#, SECRET_KEY, JWT_SECRET, API_KEY, DB_PASSWORD, generate_weak_token());

    Ok(HttpResponse::Ok().content_type("text/html").body(html))
}

// VULNERABILITY: No authentication required
async fn get_user(path: web::Path<String>) -> Result<HttpResponse> {
    let user_id = path.into_inner();
    match get_user_by_id(&user_id) {
        Ok(user) => Ok(HttpResponse::Ok().json(user)),
        Err(e) => Ok(handle_error(&*e, "Failed to get user")),
    }
}

// VULNERABILITY: Weak authentication
async fn login(req: web::Json<LoginRequest>) -> Result<HttpResponse> {
    let login_data = req.into_inner();
    
    // VULNERABILITY: No rate limiting
    // VULNERABILITY: Weak password hashing
    let hashed_password = weak_hash_password(&login_data.password);
    
    // VULNERABILITY: Hardcoded user check
    if login_data.username == "admin" && hashed_password == "5d41402abc4b2a76b9719d911017c592" {
        Ok(HttpResponse::Ok().json(json!({
            "token": generate_weak_token(),
            "user_id": 1,
            "username": login_data.username,
            "password_hash": hashed_password
        })))
    } else {
        Ok(HttpResponse::Unauthorized().json(json!({
            "error": "Invalid credentials",
            "username": login_data.username,
            "password_hash": hashed_password
        })))
    }
}

// VULNERABILITY: Command injection endpoint
async fn execute_command_endpoint(req: web::Json<CommandRequest>) -> Result<HttpResponse> {
    let request = req.into_inner();
    
    // VULNERABILITY: No input validation or sanitization
    match execute_command(&request.command) {
        Ok(output) => Ok(HttpResponse::Ok().json(json!({
            "command": request.command,
            "output": output
        }))),
        Err(e) => Ok(handle_error(&*e, "Command execution failed")),
    }
}

// VULNERABILITY: Path traversal endpoint
async fn read_file_endpoint(req: web::Json<FileRequest>) -> Result<HttpResponse> {
    let request = req.into_inner();
    
    // VULNERABILITY: No path validation
    match read_file(&request.filename) {
        Ok(content) => Ok(HttpResponse::Ok().json(json!({
            "filename": request.filename,
            "content": content
        }))),
        Err(e) => Ok(handle_error(&*e, "Failed to read file")),
    }
}

// VULNERABILITY: Encryption endpoint with weak encryption
async fn encrypt_data(req: web::Json<EncryptRequest>) -> Result<HttpResponse> {
    let request = req.into_inner();
    let encrypted = weak_encrypt(&request.data);
    
    Ok(HttpResponse::Ok().json(json!({
        "original": request.data,
        "encrypted": encrypted
    })))
}

// VULNERABILITY: Decryption endpoint
async fn decrypt_data(req: web::Json<DecryptRequest>) -> Result<HttpResponse> {
    let request = req.into_inner();
    
    match weak_decrypt(&request.encrypted_data) {
        Ok(decrypted) => Ok(HttpResponse::Ok().json(json!({
            "encrypted": request.encrypted_data,
            "decrypted": decrypted
        }))),
        Err(e) => Ok(handle_error(&*e, "Decryption failed")),
    }
}

// VULNERABILITY: Information disclosure endpoint
async fn debug_info() -> Result<HttpResponse> {
    Ok(HttpResponse::Ok().json(json!({
        "secrets": {
            "secret_key": SECRET_KEY,
            "jwt_secret": JWT_SECRET,
            "api_key": API_KEY,
            "db_password": DB_PASSWORD,
            "encryption_key": ENCRYPTION_KEY
        },
        "environment": env::vars().collect::<HashMap<String, String>>(),
        "process_id": process::id(),
        "working_dir": env::current_dir().unwrap_or_default(),
        "args": env::args().collect::<Vec<String>>()
    })))
}

// VULNERABILITY: No authentication middleware
async fn admin_panel() -> Result<HttpResponse> {
    Ok(HttpResponse::Ok().json(json!({
        "message": "Admin panel accessed without authentication",
        "users": "All user data would be here"
    })))
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    // VULNERABILITY: Running in debug mode
    env::set_var("RUST_LOG", "debug");
    env_logger::init();
    
    // Initialize global state
    unsafe {
        GLOBAL_STATE = Some(HashMap::new());
    }
    
    println!("Starting vulnerable Rust application on :8080");
    
    // VULNERABILITY: Running on all interfaces
    HttpServer::new(|| {
        App::new()
            .wrap(Logger::default())
            .route("/", web::get().to(index))
            .route("/users/{id}", web::get().to(get_user))
            .route("/login", web::post().to(login))
            .route("/execute", web::post().to(execute_command_endpoint))
            .route("/read", web::post().to(read_file_endpoint))
            .route("/encrypt", web::post().to(encrypt_data))
            .route("/decrypt", web::post().to(decrypt_data))
            .route("/debug", web::get().to(debug_info))
            .route("/admin", web::get().to(admin_panel))
    })
    .bind("0.0.0.0:8080")?
    .run()
    .await
}
