package main

import (
	"crypto/md5"
	"crypto/rand"
	"database/sql"
	"encoding/base64"
	"encoding/json"
	"fmt"
	"html/template"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/golang-jwt/jwt"
	_ "github.com/mattn/go-sqlite3"
	"golang.org/x/crypto/bcrypt"
)

// VULNERABILITY: Hardcoded secrets and credentials
const (
	SECRET_KEY     = "super-secret-key-12345"
	JWT_SECRET     = "jwt-secret-key-very-insecure"
	DB_PASSWORD    = "password123"
	API_KEY        = "sk-1234567890abcdef"
	ENCRYPTION_KEY = "my-very-weak-encryption-key"
)

// VULNERABILITY: Global variables with sensitive data
var (
	db *sql.DB
)

// User struct
type User struct {
	ID       int    `json:"id"`
	Username string `json:"username"`
	Password string `json:"password"`
	Email    string `json:"email"`
}

// VULNERABILITY: Weak password hashing using MD5
func weakHashPassword(password string) string {
	hash := md5.Sum([]byte(password))
	return fmt.Sprintf("%x", hash)
}

// VULNERABILITY: Insecure random number generation
func generateWeakToken() string {
	// Using time-based seed makes it predictable
	rand.Seed(time.Now().UnixNano())
	return fmt.Sprintf("%d", rand.Intn(10000))
}

// VULNERABILITY: Weak encryption using simple XOR
func weakEncrypt(data string) string {
	key := "weakkey"
	encrypted := make([]byte, len(data))
	for i := 0; i < len(data); i++ {
		encrypted[i] = data[i] ^ key[i%len(key)]
	}
	return base64.StdEncoding.EncodeToString(encrypted)
}

// VULNERABILITY: Weak decryption
func weakDecrypt(encryptedData string) string {
	data, err := base64.StdEncoding.DecodeString(encryptedData)
	if err != nil {
		return ""
	}
	key := "weakkey"
	decrypted := make([]byte, len(data))
	for i := 0; i < len(data); i++ {
		decrypted[i] = data[i] ^ key[i%len(key)]
	}
	return string(decrypted)
}

// VULNERABILITY: SQL Injection vulnerable function
func getUserByID(userID string) (*User, error) {
	// VULNERABILITY: Direct string concatenation without parameterization
	query := fmt.Sprintf("SELECT id, username, password, email FROM users WHERE id = %s", userID)
	row := db.QueryRow(query)
	
	user := &User{}
	err := row.Scan(&user.ID, &user.Username, &user.Password, &user.Email)
	if err != nil {
		return nil, err
	}
	return user, nil
}

// VULNERABILITY: SQL Injection in user creation
func createUser(username, password, email string) error {
	// VULNERABILITY: Direct string concatenation
	query := fmt.Sprintf("INSERT INTO users (username, password, email) VALUES ('%s', '%s', '%s')", username, password, email)
	_, err := db.Exec(query)
	return err
}

// VULNERABILITY: Command injection
func executeCommand(command string) (string, error) {
	// VULNERABILITY: Using shell execution without sanitization
	cmd := exec.Command("sh", "-c", command)
	output, err := cmd.Output()
	return string(output), err
}

// VULNERABILITY: Path traversal
func readFile(filename string) (string, error) {
	// VULNERABILITY: No path validation
	content, err := os.ReadFile(filename)
	return string(content), err
}

// VULNERABILITY: Insecure JWT token generation
func generateJWT(userID int) (string, error) {
	token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.MapClaims{
		"user_id": userID,
		"exp":     time.Now().Add(time.Hour * 24).Unix(),
	})
	// VULNERABILITY: Using weak secret
	return token.SignedString([]byte(JWT_SECRET))
}

// VULNERABILITY: Information disclosure in error handling
func handleError(c *gin.Context, err error, message string) {
	// VULNERABILITY: Exposing detailed error information
	c.JSON(http.StatusInternalServerError, gin.H{
		"error":   message,
		"details": err.Error(),
		"type":    fmt.Sprintf("%T", err),
		"file":    "main.go",
		"line":    "error_handling",
	})
}

func init() {
	// Initialize database
	var err error
	db, err = sql.Open("sqlite3", "./vulnerable_app.db")
	if err != nil {
		log.Fatal(err)
	}

	// Create users table
	createTableSQL := `
	CREATE TABLE IF NOT EXISTS users (
		id INTEGER PRIMARY KEY AUTOINCREMENT,
		username TEXT UNIQUE,
		password TEXT,
		email TEXT
	);
	`
	_, err = db.Exec(createTableSQL)
	if err != nil {
		log.Fatal(err)
	}

	// Insert default user
	_, err = db.Exec("INSERT OR IGNORE INTO users (username, password, email) VALUES ('admin', '5d41402abc4b2a76b9719d911017c592', 'admin@example.com')")
	if err != nil {
		log.Fatal(err)
	}
}

func main() {
	// VULNERABILITY: Running in debug mode
	gin.SetMode(gin.DebugMode)
	r := gin.Default()

	// VULNERABILITY: Information disclosure in main page
	r.GET("/", func(c *gin.Context) {
		c.HTML(http.StatusOK, "index.html", gin.H{
			"title":        "Vulnerable Go App",
			"secret_key":   SECRET_KEY,
			"jwt_secret":   JWT_SECRET,
			"api_key":      API_KEY,
			"db_password":  DB_PASSWORD,
			"session_id":   generateWeakToken(),
		})
	})

	// VULNERABILITY: No authentication required
	r.GET("/users/:id", func(c *gin.Context) {
		userID := c.Param("id")
		user, err := getUserByID(userID)
		if err != nil {
			handleError(c, err, "Failed to get user")
			return
		}
		c.JSON(http.StatusOK, user)
	})

	// VULNERABILITY: Insecure user registration
	r.POST("/register", func(c *gin.Context) {
		var user User
		if err := c.ShouldBindJSON(&user); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		// VULNERABILITY: No input validation
		// VULNERABILITY: Weak password hashing
		user.Password = weakHashPassword(user.Password)

		err := createUser(user.Username, user.Password, user.Email)
		if err != nil {
			handleError(c, err, "Failed to create user")
			return
		}

		c.JSON(http.StatusOK, gin.H{
			"message":        "User created successfully",
			"password_hash":  user.Password,
			"sql_query":      fmt.Sprintf("INSERT INTO users (username, password, email) VALUES ('%s', '%s', '%s')", user.Username, user.Password, user.Email),
		})
	})

	// VULNERABILITY: Weak authentication
	r.POST("/login", func(c *gin.Context) {
		var loginData struct {
			Username string `json:"username"`
			Password string `json:"password"`
		}

		if err := c.ShouldBindJSON(&loginData); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		// VULNERABILITY: No rate limiting
		// VULNERABILITY: Weak password hashing
		hashedPassword := weakHashPassword(loginData.Password)

		// VULNERABILITY: Hardcoded user check
		if loginData.Username == "admin" && hashedPassword == "5d41402abc4b2a76b9719d911017c592" {
			token, err := generateJWT(1)
			if err != nil {
				handleError(c, err, "Failed to generate token")
				return
			}

			c.JSON(http.StatusOK, gin.H{
				"token":         token,
				"user_id":       1,
				"username":      loginData.Username,
				"password_hash": hashedPassword,
			})
		} else {
			c.JSON(http.StatusUnauthorized, gin.H{
				"error":         "Invalid credentials",
				"username":      loginData.Username,
				"password_hash": hashedPassword,
			})
		}
	})

	// VULNERABILITY: Command injection endpoint
	r.POST("/execute", func(c *gin.Context) {
		var request struct {
			Command string `json:"command"`
		}

		if err := c.ShouldBindJSON(&request); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		// VULNERABILITY: No input validation or sanitization
		output, err := executeCommand(request.Command)
		if err != nil {
			handleError(c, err, "Command execution failed")
			return
		}

		c.JSON(http.StatusOK, gin.H{
			"command": request.Command,
			"output":  output,
		})
	})

	// VULNERABILITY: Path traversal endpoint
	r.POST("/read", func(c *gin.Context) {
		var request struct {
			Filename string `json:"filename"`
		}

		if err := c.ShouldBindJSON(&request); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		// VULNERABILITY: No path validation
		content, err := readFile(request.Filename)
		if err != nil {
			handleError(c, err, "Failed to read file")
			return
		}

		c.JSON(http.StatusOK, gin.H{
			"filename": request.Filename,
			"content":  content,
		})
	})

	// VULNERABILITY: Encryption endpoint with weak encryption
	r.POST("/encrypt", func(c *gin.Context) {
		var request struct {
			Data string `json:"data"`
		}

		if err := c.ShouldBindJSON(&request); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		encrypted := weakEncrypt(request.Data)
		c.JSON(http.StatusOK, gin.H{
			"original":  request.Data,
			"encrypted": encrypted,
		})
	})

	// VULNERABILITY: Decryption endpoint
	r.POST("/decrypt", func(c *gin.Context) {
		var request struct {
			EncryptedData string `json:"encrypted_data"`
		}

		if err := c.ShouldBindJSON(&request); err != nil {
			handleError(c, err, "Invalid JSON")
			return
		}

		decrypted := weakDecrypt(request.EncryptedData)
		c.JSON(http.StatusOK, gin.H{
			"encrypted": request.EncryptedData,
			"decrypted": decrypted,
		})
	})

	// VULNERABILITY: Information disclosure endpoint
	r.GET("/debug", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"secrets": gin.H{
				"secret_key":     SECRET_KEY,
				"jwt_secret":     JWT_SECRET,
				"api_key":        API_KEY,
				"db_password":    DB_PASSWORD,
				"encryption_key": ENCRYPTION_KEY,
			},
			"environment": os.Environ(),
			"process_id":  os.Getpid(),
			"working_dir": func() string {
				wd, _ := os.Getwd()
				return wd
			}(),
		})
	})

	// VULNERABILITY: No authentication middleware
	r.GET("/admin", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"message": "Admin panel accessed without authentication",
			"users":   "All user data would be here",
		})
	})

	// Load HTML templates
	r.LoadHTMLGlob("templates/*")

	// VULNERABILITY: Running on all interfaces
	log.Println("Starting vulnerable Go application on :8080")
	log.Fatal(r.Run(":8080"))
}
