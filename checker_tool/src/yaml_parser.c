#include "yaml_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

static void trim_whitespace(char* str) {
    char* start = str;
    char* end = str + strlen(str) - 1;
    
    // Trim leading whitespace
    while (isspace(*start)) start++;
    
    // Trim trailing whitespace
    while (end > start && isspace(*end)) end--;
    
    // Null terminate
    *(end + 1) = '\0';
    
    // Move trimmed string to beginning
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

static int is_section_header(const char* line) {
    return (strlen(line) > 0 && line[0] != ' ' && line[0] != '\t' && 
            line[strlen(line) - 1] == ':');
}

static int parse_key_value(const char* line, char* key, char* value) {
    const char* colon = strchr(line, ':');
    if (!colon) return 0;
    
    size_t key_len = colon - line;
    if (key_len >= MAX_STRING_LEN) return 0;
    
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim_whitespace(key);
    
    const char* value_start = colon + 1;
    while (*value_start && isspace(*value_start)) value_start++;
    
    if (*value_start == '"') {
        // Quoted string
        value_start++;
        const char* value_end = strrchr(value_start, '"');
        if (!value_end) return 0;
        
        size_t value_len = value_end - value_start;
        if (value_len >= MAX_STRING_LEN) return 0;
        
        strncpy(value, value_start, value_len);
        value[value_len] = '\0';
    } else {
        // Unquoted string
        strncpy(value, value_start, MAX_STRING_LEN - 1);
        value[MAX_STRING_LEN - 1] = '\0';
        trim_whitespace(value);
    }
    
    return 1;
}

static int parse_list_item(const char* line, char* item) {
    if (line[0] != '-') return 0;
    
    const char* item_start = line + 1;
    while (*item_start && isspace(*item_start)) item_start++;
    
    if (*item_start == '"') {
        // Quoted string
        item_start++;
        const char* item_end = strrchr(item_start, '"');
        if (!item_end) return 0;
        
        size_t item_len = item_end - item_start;
        if (item_len >= MAX_STRING_LEN) return 0;
        
        strncpy(item, item_start, item_len);
        item[item_len] = '\0';
    } else {
        // Unquoted string
        strncpy(item, item_start, MAX_STRING_LEN - 1);
        item[MAX_STRING_LEN - 1] = '\0';
        trim_whitespace(item);
    }
    
    return 1;
}

static int parse_approved_sdk(const char* line, approved_sdk_t* sdk) {
    // Expected format: - name: "react" versions: ">=18.0.0"
    if (line[0] != '-') return 0;
    
    const char* name_start = strstr(line, "name:");
    if (!name_start) return 0;
    name_start += 5; // Skip "name:"
    while (*name_start && isspace(*name_start)) name_start++;
    
    if (*name_start == '"') {
        name_start++;
        const char* name_end = strchr(name_start, '"');
        if (!name_end) return 0;
        
        size_t name_len = name_end - name_start;
        if (name_len >= MAX_STRING_LEN) return 0;
        
        strncpy(sdk->name, name_start, name_len);
        sdk->name[name_len] = '\0';
    }
    
    const char* versions_start = strstr(line, "versions:");
    if (!versions_start) return 0;
    versions_start += 8; // Skip "versions:"
    while (*versions_start && isspace(*versions_start)) versions_start++;
    
    if (*versions_start == '"') {
        versions_start++;
        const char* versions_end = strrchr(versions_start, '"');
        if (!versions_end) return 0;
        
        size_t versions_len = versions_end - versions_start;
        if (versions_len >= MAX_STRING_LEN) return 0;
        
        strncpy(sdk->versions, versions_start, versions_len);
        sdk->versions[versions_len] = '\0';
    }
    
    return 1;
}

severity_level_t parse_severity(const char* severity_str) {
    if (strcasecmp(severity_str, "CRITICAL") == 0) return SEVERITY_CRITICAL;
    if (strcasecmp(severity_str, "HIGH") == 0) return SEVERITY_HIGH;
    if (strcasecmp(severity_str, "MEDIUM") == 0) return SEVERITY_MEDIUM;
    if (strcasecmp(severity_str, "LOW") == 0) return SEVERITY_LOW;
    return SEVERITY_MEDIUM; // Default
}

const char* severity_to_string(severity_level_t severity) {
    switch (severity) {
        case SEVERITY_CRITICAL: return "CRITICAL";
        case SEVERITY_HIGH: return "HIGH";
        case SEVERITY_MEDIUM: return "MEDIUM";
        case SEVERITY_LOW: return "LOW";
        default: return "UNKNOWN";
    }
}

int parse_security_yaml(const char* filename, security_policy_t* policy) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return 1;
    }
    
    // Initialize policy with defaults
    memset(policy, 0, sizeof(security_policy_t));
    policy->failure_threshold = SEVERITY_HIGH;
    policy->hardcoded_secret_scan.enabled = 1;
    policy->hardcoded_secret_scan.entropy_threshold = 4.5;
    policy->binary_checks.strip_symbols = 1;
    policy->binary_checks.pie_enabled = 1;
    policy->binary_checks.stack_canary = 1;
    policy->memory_safety_checks = 1;
    policy->concurrency_checks = 1;
    
    char line[2048];
    char current_section[256] = "";
    int in_approved_sdks = 0;
    int in_blacklisted_versions = 0;
    int in_hardening_flags = 0;
    
    while (fgets(line, sizeof(line), file)) {
        trim_whitespace(line);
        
        // Skip empty lines and comments
        if (strlen(line) == 0 || line[0] == '#') continue;
        
        // Check for section headers
        if (is_section_header(line)) {
            strncpy(current_section, line, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
            // Remove trailing colon
            char* colon = strrchr(current_section, ':');
            if (colon) *colon = '\0';
            
            in_approved_sdks = (strcmp(current_section, "approved_sdks") == 0);
            in_blacklisted_versions = (strcmp(current_section, "blacklisted_versions") == 0);
            in_hardening_flags = (strcmp(current_section, "required_hardening_flags") == 0);
            continue;
        }
        
        // Parse based on current section
        if (in_approved_sdks && line[0] == '-') {
            if (policy->approved_sdk_count < MAX_SDK_ENTRIES) {
                if (parse_approved_sdk(line, &policy->approved_sdks[policy->approved_sdk_count])) {
                    policy->approved_sdk_count++;
                }
            }
        } else if (in_blacklisted_versions && line[0] == '-') {
            if (policy->blacklisted_version_count < MAX_LIST_SIZE) {
                char item[MAX_STRING_LEN];
                if (parse_list_item(line, item)) {
                    strncpy(policy->blacklisted_versions[policy->blacklisted_version_count].version,
                           item, MAX_STRING_LEN - 1);
                    policy->blacklisted_version_count++;
                }
            }
        } else if (in_hardening_flags && line[0] == '-') {
            if (policy->hardening_flags_count < MAX_LIST_SIZE) {
                char flag[MAX_STRING_LEN];
                if (parse_list_item(line, flag)) {
                    strncpy(policy->required_hardening_flags[policy->hardening_flags_count],
                           flag, MAX_STRING_LEN - 1);
                    policy->hardening_flags_count++;
                }
            }
        } else {
            // Parse key-value pairs
            char key[MAX_STRING_LEN];
            char value[MAX_STRING_LEN];
            
            if (parse_key_value(line, key, value)) {
                if (strcmp(key, "failure_threshold") == 0) {
                    policy->failure_threshold = parse_severity(value);
                } else if (strcmp(key, "symmetric_encryption") == 0) {
                    strncpy(policy->minimum_standards.symmetric_encryption, value, MAX_STRING_LEN - 1);
                } else if (strcmp(key, "hashing") == 0) {
                    strncpy(policy->minimum_standards.hashing, value, MAX_STRING_LEN - 1);
                } else if (strcmp(key, "tls_version") == 0) {
                    strncpy(policy->minimum_standards.tls_version, value, MAX_STRING_LEN - 1);
                } else if (strcmp(key, "enabled") == 0) {
                    if (strcmp(current_section, "hardcoded_secret_scan") == 0) {
                        policy->hardcoded_secret_scan.enabled = (strcasecmp(value, "true") == 0);
                    }
                } else if (strcmp(key, "entropy_threshold") == 0) {
                    if (strcmp(current_section, "hardcoded_secret_scan") == 0) {
                        policy->hardcoded_secret_scan.entropy_threshold = atof(value);
                    }
                } else if (strcmp(key, "strip_symbols") == 0) {
                    if (strcmp(current_section, "binary_checks") == 0) {
                        policy->binary_checks.strip_symbols = (strcasecmp(value, "true") == 0);
                    }
                } else if (strcmp(key, "pie_enabled") == 0) {
                    if (strcmp(current_section, "binary_checks") == 0) {
                        policy->binary_checks.pie_enabled = (strcasecmp(value, "true") == 0);
                    }
                } else if (strcmp(key, "stack_canary") == 0) {
                    if (strcmp(current_section, "binary_checks") == 0) {
                        policy->binary_checks.stack_canary = (strcasecmp(value, "true") == 0);
                    }
                } else if (strcmp(key, "memory_safety_checks") == 0) {
                    policy->memory_safety_checks = (strcasecmp(value, "true") == 0);
                } else if (strcmp(key, "concurrency_checks") == 0) {
                    policy->concurrency_checks = (strcasecmp(value, "true") == 0);
                }
            }
        }
    }
    
    fclose(file);
    return 0;
}

void cleanup_security_policy(security_policy_t* policy) {
    // Nothing to cleanup for this simple structure
    (void)policy;
}
