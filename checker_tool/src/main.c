#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include "yaml_parser.h"
#include "sdk_analyzer.h"
#include "crypto_scanner.h"
#include "binary_parser.h"
#include "static_analyzer.h"
#include "report_generator.h"

#define VERSION "1.0.0"
#define MAX_PATH_LEN 4096

// ANSI Color Codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

typedef struct {
    char target_path[MAX_PATH_LEN];
    char security_yaml_path[MAX_PATH_LEN];
    char html_output_path[MAX_PATH_LEN];
    int verbose;
    int help;
    int version;
    int html_export;
} cli_args_t;

void print_usage(const char* program_name) {
    printf("MasterFabric Pre-Release Security Checker v%s\n", VERSION);
    printf("Usage: %s --check [TARGET_PATH]\n", program_name);
    printf("\n");
    printf("Options:\n");
    printf("  --check [PATH]    Check the specified directory for security issues\n");
    printf("                    If PATH is omitted, checks current directory\n");
    printf("  --html [FILE]     Export results to HTML file\n");
    printf("                    If FILE is omitted, uses 'security_report.html'\n");
    printf("  --verbose         Enable verbose output\n");
    printf("  --help            Show this help message\n");
    printf("  --version         Show version information\n");
    printf("\n");
    printf("The tool looks for a .security.yaml file in the target directory\n");
    printf("and performs comprehensive security analysis based on the policy.\n");
    printf("\n");
    printf("Exit codes:\n");
    printf("  0  - All checks passed\n");
    printf("  1  - Policy violations found\n");
    printf("  2  - Configuration error\n");
    printf("  3  - Scan error\n");
}

void print_version() {
    printf("MasterFabric Pre-Release Security Checker v%s\n", VERSION);
}

int parse_arguments(int argc, char* argv[], cli_args_t* args) {
    memset(args, 0, sizeof(cli_args_t));
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            args->help = 1;
            return 0;
        } else if (strcmp(argv[i], "--version") == 0) {
            args->version = 1;
            return 0;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            args->verbose = 1;
        } else if (strcmp(argv[i], "--html") == 0) {
            args->html_export = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                strncpy(args->html_output_path, argv[i + 1], MAX_PATH_LEN - 1);
                i++; // Skip the file argument
            } else {
                // No file provided, use default
                strcpy(args->html_output_path, "security_report.html");
            }
        } else if (strcmp(argv[i], "--check") == 0) {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                strncpy(args->target_path, argv[i + 1], MAX_PATH_LEN - 1);
                i++; // Skip the path argument
            } else {
                // No path provided, use current directory
                if (getcwd(args->target_path, MAX_PATH_LEN) == NULL) {
                    fprintf(stderr, "Error: Could not get current directory\n");
                    return 2;
                }
            }
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            return 2;
        }
    }
    
    // If no --check flag was provided, show help
    if (strlen(args->target_path) == 0 && !args->help) {
        print_usage(argv[0]);
        return 2;
    }
    
    return 0;
}

int validate_target_path(const char* path) {
    struct stat path_stat;
    
    if (stat(path, &path_stat) != 0) {
        fprintf(stderr, "Error: Target path '%s' does not exist: %s\n", path, strerror(errno));
        return 2;
    }
    
    if (!S_ISDIR(path_stat.st_mode)) {
        fprintf(stderr, "Error: Target path '%s' is not a directory\n", path);
        return 2;
    }
    
    return 0;
}

int find_security_yaml(const char* target_path, char* yaml_path) {
    snprintf(yaml_path, MAX_PATH_LEN, "%s/.security.yaml", target_path);
    
    struct stat yaml_stat;
    if (stat(yaml_path, &yaml_stat) == 0) {
        return 0; // Found
    }
    
    // Try alternative names
    snprintf(yaml_path, MAX_PATH_LEN, "%s/security_policy.security.yaml", target_path);
    if (stat(yaml_path, &yaml_stat) == 0) {
        return 0; // Found
    }
    
    return 1; // Not found
}

int main(int argc, char* argv[]) {
    cli_args_t args;
    int parse_result = parse_arguments(argc, argv, &args);
    
    if (parse_result != 0) {
        if (args.help) {
            print_usage(argv[0]);
            return 0;
        }
        return parse_result;
    }
    
    // Handle help and version commands
    if (args.help) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (args.version) {
        print_version();
        return 0;
    }
    
    if (args.verbose) {
        print_header();
        printf("%s%sTarget directory: %s%s%s\n", BLUE, BOLD, CYAN, args.target_path, RESET);
    }
    
    // Validate target path
    if (validate_target_path(args.target_path) != 0) {
        return 2;
    }
    
    // Find security.yaml file
    if (find_security_yaml(args.target_path, args.security_yaml_path) != 0) {
        fprintf(stderr, "Error: No .security.yaml file found in target directory\n");
        fprintf(stderr, "Expected: %s/.security.yaml or %s/security_policy.security.yaml\n", 
                args.target_path, args.target_path);
        return 2;
    }
    
    if (args.verbose) {
        printf("Security policy file: %s\n", args.security_yaml_path);
    }
    
    // Initialize security policy
    security_policy_t policy;
    if (parse_security_yaml(args.security_yaml_path, &policy) != 0) {
        fprintf(stderr, "Error: Failed to parse security policy file\n");
        return 2;
    }
    
    if (args.verbose) {
        printf("%s%sSecurity policy loaded successfully%s\n", GREEN, BOLD, RESET);
        printf("%s%sFailure threshold: %s%s%s\n", YELLOW, BOLD, CYAN, severity_to_string(policy.failure_threshold), RESET);
    }
    
    // Initialize scan results
    scan_results_t results;
    memset(&results, 0, sizeof(scan_results_t));
    int api_calls_made = 0;
    
    // Perform security checks with loading animations
    printf("\n%s%sStarting security analysis...%s\n", BLUE, BOLD, RESET);
    
    // 1. SDK and dependency analysis
    if (args.verbose) {
        printf("\n");
        print_loading_animation("Checking dependencies and SDKs");
        printf("\n");
    }
    if (analyze_dependencies(args.target_path, &policy, &results) != 0) {
        fprintf(stderr, "Warning: Dependency analysis failed\n");
    }
    api_calls_made += 6; // Estimate based on typical API calls
    
    // 2. Cryptographic analysis
    if (args.verbose) {
        printf("\n");
        print_loading_animation("Scanning for cryptographic issues");
        printf("\n");
    }
    if (scan_cryptographic_issues(args.target_path, &policy, &results) != 0) {
        fprintf(stderr, "Warning: Cryptographic analysis failed\n");
    }
    
    // 3. Binary hardening analysis
    if (args.verbose) {
        printf("\n");
        print_loading_animation("Checking binary hardening");
        printf("\n");
    }
    if (check_binary_hardening(args.target_path, &policy, &results) != 0) {
        fprintf(stderr, "Warning: Binary hardening analysis failed\n");
    }
    
    // 4. Static analysis
    if (args.verbose) {
        printf("\n");
        print_loading_animation("Running static analysis");
        printf("\n");
    }
    if (run_static_analysis(args.target_path, &policy, &results) != 0) {
        fprintf(stderr, "Warning: Static analysis failed\n");
    }
    
    // Generate and display enhanced report
    generate_security_report(&results, &policy, args.verbose, api_calls_made);
    
    // Export to HTML if requested
    if (args.html_export) {
        if (generate_html_report(&results, &policy, args.html_output_path, api_calls_made) == 0) {
            printf("\n%s✓ HTML report exported to: %s%s\n", GREEN, args.html_output_path, RESET);
        } else {
            fprintf(stderr, "Error: Failed to generate HTML report\n");
        }
    }
    
    // Determine exit code based on results and policy
    int exit_code = determine_exit_code(&results, &policy);
    
    if (exit_code == 0) {
        printf("\n✓ All security checks passed!\n");
    } else if (exit_code == 1) {
        printf("\n✗ Security policy violations found!\n");
    }
    
    // Cleanup
    cleanup_scan_results(&results);
    cleanup_security_policy(&policy);
    
    return exit_code;
}
