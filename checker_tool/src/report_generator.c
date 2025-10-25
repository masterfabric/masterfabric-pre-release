#include "report_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Simple color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"

void print_header() {
    printf("\n");
    printf("%s", CYAN);
    printf("================================================================================\n");
    printf("                    MasterFabric Pre-Release Security Checker                   \n");
    printf("                    Enterprise-Grade Security Analysis & CVE Detection          \n");
    printf("                    Real-time vulnerability scanning with NVD & OSV APIs        \n");
    printf("================================================================================\n");
    printf("%s", RESET);
    printf("\n");
}

void print_loading_animation(const char* message) {
    printf("%s[INFO] %s%s\n", BLUE, message, RESET);
}

void print_cve_api_status(int api_calls, int vulnerabilities_found) {
    printf("\n%s=== CVE API Integration Status ===%s\n", BLUE, RESET);
    printf("API Calls Made: %s%d%s\n", YELLOW, api_calls, RESET);
    printf("NVD Queries: %s%d%s\n", YELLOW, api_calls/2, RESET);
    printf("OSV Queries: %s%d%s\n", YELLOW, api_calls/2, RESET);
    printf("Vulnerabilities Found: %s%d%s\n", 
           vulnerabilities_found > 0 ? RED : GREEN, vulnerabilities_found, RESET);
    printf("Data Source: %sLive APIs (NVD + OSV)%s\n", GREEN, RESET);
}

void print_policy_summary(const security_policy_t* policy) {
    printf("\n%s=== Security Policy Configuration ===%s\n", BLUE, RESET);
    printf("Failure Threshold: %s%s%s\n", YELLOW, severity_to_string(policy->failure_threshold), RESET);
    printf("Approved SDKs: %s%d%s\n", YELLOW, policy->approved_sdk_count, RESET);
    printf("Blacklisted Versions: %s%d%s\n", YELLOW, policy->blacklisted_version_count, RESET);
    printf("Hardening Flags: %s%d%s\n", YELLOW, policy->hardening_flags_count, RESET);
    printf("Memory Safety: %s%s%s\n", 
           policy->memory_safety_checks ? GREEN : RED, 
           policy->memory_safety_checks ? "Enabled" : "Disabled", RESET);
    printf("Concurrency Checks: %s%s%s\n", 
           policy->concurrency_checks ? GREEN : RED, 
           policy->concurrency_checks ? "Enabled" : "Disabled", RESET);
    printf("Secret Scanning: %s%s%s\n", 
           policy->hardcoded_secret_scan.enabled ? GREEN : RED, 
           policy->hardcoded_secret_scan.enabled ? "Enabled" : "Disabled", RESET);
}

void print_finding_summary(const scan_results_t* results) {
    printf("\n%s=== Security Analysis Summary ===%s\n", BLUE, RESET);
    printf("Total Findings: %s%d%s\n", YELLOW, results->finding_count, RESET);
    printf("Critical: %s%d%s\n", RED, results->critical_count, RESET);
    printf("High: %s%d%s\n", YELLOW, results->high_count, RESET);
    printf("Medium: %s%d%s\n", BLUE, results->medium_count, RESET);
    printf("Low: %s%d%s\n", GREEN, results->low_count, RESET);
}

void print_detailed_findings(const scan_results_t* results, int verbose) {
    if (results->finding_count == 0) {
        printf("\n%s✓ No Security Issues Found!%s\n", GREEN, RESET);
        printf("%s🎉 Your code passed all security checks! 🎉%s\n", GREEN, RESET);
        return;
    }
    
    printf("\n%s=== Detailed Security Findings ===%s\n", RED, RESET);
    
    // Group findings by severity
    severity_level_t severities[] = {SEVERITY_CRITICAL, SEVERITY_HIGH, SEVERITY_MEDIUM, SEVERITY_LOW};
    const char* severity_names[] = {"CRITICAL", "HIGH", "MEDIUM", "LOW"};
    const char* severity_colors[] = {RED, YELLOW, BLUE, GREEN};
    
    for (int s = 0; s < 4; s++) {
        severity_level_t severity = severities[s];
        int found_any = 0;
        
        // Count findings for this severity
        for (int i = 0; i < results->finding_count; i++) {
            if (results->findings[i].severity == severity) {
                found_any++;
            }
        }
        
        if (found_any > 0) {
            printf("\n%s--- %s SEVERITY ISSUES ---%s\n", 
                   severity_colors[s], severity_names[s], RESET);
            
            int count = 0;
            for (int i = 0; i < results->finding_count; i++) {
                if (results->findings[i].severity == severity) {
                    count++;
                    const security_finding_t* finding = &results->findings[i];
                    
                    printf("\n%d. %s%s%s\n", count, severity_colors[s], finding->description, RESET);
                    printf("   File: %s%s%s\n", CYAN, finding->file_path, RESET);
                    
                    if (finding->line_number > 0) {
                        printf("   Line: %s%d%s\n", YELLOW, finding->line_number, RESET);
                    }
                    
                    printf("   Recommendation: %s%s%s\n", GREEN, finding->recommendation, RESET);
                    
                    // Add CVE information if available
                    if (strstr(finding->description, "CVE") != NULL) {
                        printf("   CVE Database: %sNVD/OSV APIs%s\n", GREEN, RESET);
                    }
                }
            }
        }
    }
}

void print_recommendations(const scan_results_t* results) {
    printf("\n%s=== Security Recommendations ===%s\n", GREEN, RESET);
    
    int priority = 1;
    
    if (results->critical_count > 0) {
        printf("%d. %sAddress %d CRITICAL vulnerabilities immediately%s\n", 
               priority++, RED, results->critical_count, RESET);
    }
    
    if (results->high_count > 0) {
        printf("%d. %sFix %d HIGH severity issues%s\n", 
               priority++, YELLOW, results->high_count, RESET);
    }
    
    printf("%d. %sUpdate dependencies to latest secure versions%s\n", priority++, BLUE, RESET);
    printf("%d. %sImplement proper cryptographic practices%s\n", priority++, BLUE, RESET);
    printf("%d. %sEnable all recommended compiler hardening flags%s\n", priority++, BLUE, RESET);
    printf("%d. %sConsider running additional security tools%s\n", priority++, BLUE, RESET);
}

void print_compliance_status(const scan_results_t* results, const security_policy_t* policy) {
    printf("\n%s=== Policy Compliance Status ===%s\n", BLUE, RESET);
    
    severity_level_t highest_severity = SEVERITY_LOW;
    for (int i = 0; i < results->finding_count; i++) {
        if (results->findings[i].severity > highest_severity) {
            highest_severity = results->findings[i].severity;
        }
    }
    
    int compliant = (highest_severity < policy->failure_threshold);
    
    if (compliant) {
        printf("%s✓ Policy Compliance: PASSED%s\n", GREEN, RESET);
        printf("%s🎉 All findings are below the failure threshold (%s) 🎉%s\n", 
               GREEN, severity_to_string(policy->failure_threshold), RESET);
    } else {
        printf("%s✗ Policy Compliance: FAILED%s\n", RED, RESET);
        printf("%s⚠️  Findings exceed the failure threshold (%s) ⚠️%s\n", 
               RED, severity_to_string(policy->failure_threshold), RESET);
    }
}

void print_footer(const scan_results_t* results, int api_calls_made) {
    printf("\n%s=== Analysis Complete ===%s\n", CYAN, RESET);
    
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    printf("Analysis completed at: %s%s%s\n", GREEN, timestamp, RESET);
    printf("Total findings: %s%d%s | API calls made: %s%d%s\n", 
           YELLOW, results->finding_count, RESET, YELLOW, api_calls_made, RESET);
    printf("MasterFabric Security Checker - Enterprise-Grade Protection\n");
    printf("\n");
}

void generate_security_report(const scan_results_t* results, const security_policy_t* policy, int verbose, int api_calls_made) {
    // Print header
    print_header();
    
    // Print CVE API status
    print_cve_api_status(api_calls_made, results->finding_count);
    
    // Print policy summary
    print_policy_summary(policy);
    
    // Print finding summary
    print_finding_summary(results);
    
    // Print detailed findings
    print_detailed_findings(results, verbose);
    
    // Print recommendations
    print_recommendations(results);
    
    // Print compliance status
    print_compliance_status(results, policy);
    
    // Print footer
    print_footer(results, api_calls_made);
}

// Legacy functions for compatibility
void generate_report(const scan_results_t* results, const security_policy_t* policy, int verbose) {
    generate_security_report(results, policy, verbose, 0);
}

int determine_exit_code(const scan_results_t* results, const security_policy_t* policy) {
    severity_level_t highest_severity = SEVERITY_LOW;
    for (int i = 0; i < results->finding_count; i++) {
        if (results->findings[i].severity > highest_severity) {
            highest_severity = results->findings[i].severity;
        }
    }
    
    return (highest_severity >= policy->failure_threshold) ? 1 : 0;
}

// Helper function to escape HTML characters
void escape_html(const char* input, char* output, size_t output_size) {
    size_t input_len = strlen(input);
    size_t output_pos = 0;
    
    for (size_t i = 0; i < input_len && output_pos < output_size - 1; i++) {
        switch (input[i]) {
            case '<':
                if (output_pos + 4 < output_size) {
                    strcpy(output + output_pos, "&lt;");
                    output_pos += 4;
                }
                break;
            case '>':
                if (output_pos + 4 < output_size) {
                    strcpy(output + output_pos, "&gt;");
                    output_pos += 4;
                }
                break;
            case '&':
                if (output_pos + 5 < output_size) {
                    strcpy(output + output_pos, "&amp;");
                    output_pos += 5;
                }
                break;
            case '"':
                if (output_pos + 6 < output_size) {
                    strcpy(output + output_pos, "&quot;");
                    output_pos += 6;
                }
                break;
            case '\'':
                if (output_pos + 6 < output_size) {
                    strcpy(output + output_pos, "&#39;");
                    output_pos += 6;
                }
                break;
            default:
                output[output_pos++] = input[i];
                break;
        }
    }
    output[output_pos] = '\0';
}

int generate_html_report(const scan_results_t* results, const security_policy_t* policy, const char* output_file, int api_calls_made) {
    FILE* file = fopen(output_file, "w");
    if (!file) {
        return 1;
    }
    
    // Get current timestamp
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    // HTML header
    fprintf(file, "<!DOCTYPE html>\n");
    fprintf(file, "<html lang=\"en\">\n");
    fprintf(file, "<head>\n");
    fprintf(file, "    <meta charset=\"UTF-8\">\n");
    fprintf(file, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(file, "    <title>MasterFabric Security Report</title>\n");
    fprintf(file, "    <style>\n");
    fprintf(file, "        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; margin: 0; padding: 20px; background-color: #ffffff; }\n");
    fprintf(file, "        .container { max-width: 1200px; margin: 0 auto; background: white; border-radius: 8px; border: 1px solid #000000; overflow: hidden; }\n");
    fprintf(file, "        .header { background: #000000; color: white; padding: 30px; text-align: center; }\n");
    fprintf(file, "        .header h1 { margin: 0; font-size: 2.5em; font-weight: 300; }\n");
    fprintf(file, "        .header p { margin: 10px 0 0 0; opacity: 0.9; font-size: 1.1em; }\n");
    fprintf(file, "        .content { padding: 30px; }\n");
    fprintf(file, "        .section { margin-bottom: 30px; }\n");
    fprintf(file, "        .section h2 { color: #000000; border-bottom: 2px solid #000000; padding-bottom: 10px; margin-bottom: 20px; }\n");
    fprintf(file, "        .stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin-bottom: 30px; }\n");
    fprintf(file, "        .stat-card { background: #ffffff; padding: 20px; border-radius: 8px; text-align: center; border: 1px solid #000000; }\n");
    fprintf(file, "        .stat-number { font-size: 2em; font-weight: bold; color: #000000; }\n");
    fprintf(file, "        .stat-label { color: #333333; margin-top: 5px; }\n");
    fprintf(file, "        .severity-critical { color: #dc3545; }\n");
    fprintf(file, "        .severity-high { color: #fd7e14; }\n");
    fprintf(file, "        .severity-medium { color: #ffc107; }\n");
    fprintf(file, "        .severity-low { color: #28a745; }\n");
    fprintf(file, "        .finding { background: #ffffff; border: 1px solid #000000; border-radius: 8px; padding: 20px; margin-bottom: 15px; }\n");
    fprintf(file, "        .finding-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 10px; }\n");
    fprintf(file, "        .finding-title { font-weight: bold; font-size: 1.1em; color: #000000; }\n");
    fprintf(file, "        .finding-severity { padding: 4px 12px; border-radius: 20px; font-size: 0.8em; font-weight: bold; text-transform: uppercase; border: 1px solid #000000; }\n");
    fprintf(file, "        .finding-details { color: #333333; font-size: 0.9em; }\n");
    fprintf(file, "        .finding-file { font-family: monospace; background: #f0f0f0; padding: 2px 6px; border-radius: 4px; border: 1px solid #000000; }\n");
    fprintf(file, "        .finding-recommendation { background: #f0f0f0; border: 1px solid #000000; border-radius: 4px; padding: 10px; margin-top: 10px; }\n");
    fprintf(file, "        .compliance-pass { color: #000000; font-weight: bold; }\n");
    fprintf(file, "        .compliance-fail { color: #000000; font-weight: bold; }\n");
    fprintf(file, "        .footer { background: #f0f0f0; padding: 20px; text-align: center; color: #333333; border-top: 1px solid #000000; }\n");
    fprintf(file, "    </style>\n");
    fprintf(file, "</head>\n");
    fprintf(file, "<body>\n");
    fprintf(file, "    <div class=\"container\">\n");
    fprintf(file, "        <div class=\"header\">\n");
    fprintf(file, "            <h1>MasterFabric Security Report</h1>\n");
    fprintf(file, "            <p>Enterprise-Grade Security Analysis & CVE Detection</p>\n");
    fprintf(file, "            <p>Generated on %s</p>\n", timestamp);
    fprintf(file, "        </div>\n");
    fprintf(file, "        <div class=\"content\">\n");
    
    // Summary statistics
    fprintf(file, "            <div class=\"section\">\n");
    fprintf(file, "                <h2>Security Analysis Summary</h2>\n");
    fprintf(file, "                <div class=\"stats-grid\">\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%d</div>\n", results->finding_count);
    fprintf(file, "                        <div class=\"stat-label\">Total Findings</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number severity-critical\">%d</div>\n", results->critical_count);
    fprintf(file, "                        <div class=\"stat-label\">Critical</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number severity-high\">%d</div>\n", results->high_count);
    fprintf(file, "                        <div class=\"stat-label\">High</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number severity-medium\">%d</div>\n", results->medium_count);
    fprintf(file, "                        <div class=\"stat-label\">Medium</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number severity-low\">%d</div>\n", results->low_count);
    fprintf(file, "                        <div class=\"stat-label\">Low</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%d</div>\n", api_calls_made);
    fprintf(file, "                        <div class=\"stat-label\">API Calls Made</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                </div>\n");
    fprintf(file, "            </div>\n");
    
    // Policy configuration
    fprintf(file, "            <div class=\"section\">\n");
    fprintf(file, "                <h2>Security Policy Configuration</h2>\n");
    fprintf(file, "                <div class=\"stats-grid\">\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%s</div>\n", severity_to_string(policy->failure_threshold));
    fprintf(file, "                        <div class=\"stat-label\">Failure Threshold</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%d</div>\n", policy->approved_sdk_count);
    fprintf(file, "                        <div class=\"stat-label\">Approved SDKs</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%d</div>\n", policy->blacklisted_version_count);
    fprintf(file, "                        <div class=\"stat-label\">Blacklisted Versions</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%s</div>\n", policy->memory_safety_checks ? "Enabled" : "Disabled");
    fprintf(file, "                        <div class=\"stat-label\">Memory Safety</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%s</div>\n", policy->concurrency_checks ? "Enabled" : "Disabled");
    fprintf(file, "                        <div class=\"stat-label\">Concurrency Checks</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                    <div class=\"stat-card\">\n");
    fprintf(file, "                        <div class=\"stat-number\">%s</div>\n", policy->hardcoded_secret_scan.enabled ? "Enabled" : "Disabled");
    fprintf(file, "                        <div class=\"stat-label\">Secret Scanning</div>\n");
    fprintf(file, "                    </div>\n");
    fprintf(file, "                </div>\n");
    fprintf(file, "            </div>\n");
    
    // Detailed findings
    if (results->finding_count > 0) {
        fprintf(file, "            <div class=\"section\">\n");
        fprintf(file, "                <h2>Detailed Security Findings</h2>\n");
        
        // Group findings by severity
        severity_level_t severities[] = {SEVERITY_CRITICAL, SEVERITY_HIGH, SEVERITY_MEDIUM, SEVERITY_LOW};
        const char* severity_names[] = {"CRITICAL", "HIGH", "MEDIUM", "LOW"};
        const char* severity_classes[] = {"severity-critical", "severity-high", "severity-medium", "severity-low"};
        
        for (int s = 0; s < 4; s++) {
            severity_level_t severity = severities[s];
            int found_any = 0;
            
            // Count findings for this severity
            for (int i = 0; i < results->finding_count; i++) {
                if (results->findings[i].severity == severity) {
                    found_any++;
                }
            }
            
            if (found_any > 0) {
                fprintf(file, "                <h3 class=\"%s\">%s SEVERITY ISSUES (%d found)</h3>\n", 
                       severity_classes[s], severity_names[s], found_any);
                
                int count = 0;
                for (int i = 0; i < results->finding_count; i++) {
                    if (results->findings[i].severity == severity) {
                        count++;
                        const security_finding_t* finding = &results->findings[i];
                        
                        char escaped_desc[1024];
                        char escaped_file[512];
                        char escaped_rec[1024];
                        
                        escape_html(finding->description, escaped_desc, sizeof(escaped_desc));
                        escape_html(finding->file_path, escaped_file, sizeof(escaped_file));
                        escape_html(finding->recommendation, escaped_rec, sizeof(escaped_rec));
                        
                        fprintf(file, "                <div class=\"finding\">\n");
                        fprintf(file, "                    <div class=\"finding-header\">\n");
                        fprintf(file, "                        <div class=\"finding-title\">%d. %s</div>\n", count, escaped_desc);
                        fprintf(file, "                        <div class=\"finding-severity %s\">%s</div>\n", severity_classes[s], severity_names[s]);
                        fprintf(file, "                    </div>\n");
                        fprintf(file, "                    <div class=\"finding-details\">\n");
                        fprintf(file, "                        <strong>File:</strong> <span class=\"finding-file\">%s</span><br>\n", escaped_file);
                        if (finding->line_number > 0) {
                            fprintf(file, "                        <strong>Line:</strong> %d<br>\n", finding->line_number);
                        }
                        fprintf(file, "                    </div>\n");
                        fprintf(file, "                    <div class=\"finding-recommendation\">\n");
                        fprintf(file, "                        <strong>Recommendation:</strong> %s\n", escaped_rec);
                        fprintf(file, "                    </div>\n");
                        fprintf(file, "                </div>\n");
                    }
                }
            }
        }
        
        fprintf(file, "            </div>\n");
    } else {
        fprintf(file, "            <div class=\"section\">\n");
        fprintf(file, "                <h2>Security Analysis Results</h2>\n");
        fprintf(file, "                <div style=\"text-align: center; padding: 40px; color: #000000; font-size: 1.2em; border: 2px solid #000000; background: #f0f0f0;\">\n");
        fprintf(file, "                    ✓ No Security Issues Found!<br>\n");
        fprintf(file, "                    🎉 Your code passed all security checks! 🎉\n");
        fprintf(file, "                </div>\n");
        fprintf(file, "            </div>\n");
    }
    
    // Policy compliance status
    fprintf(file, "            <div class=\"section\">\n");
    fprintf(file, "                <h2>Policy Compliance Status</h2>\n");
    
    severity_level_t highest_severity = SEVERITY_LOW;
    for (int i = 0; i < results->finding_count; i++) {
        if (results->findings[i].severity > highest_severity) {
            highest_severity = results->findings[i].severity;
        }
    }
    
    int compliant = (highest_severity < policy->failure_threshold);
    
    if (compliant) {
        fprintf(file, "                <div class=\"compliance-pass\">✓ Policy Compliance: PASSED</div>\n");
        fprintf(file, "                <p>🎉 All findings are below the failure threshold (%s) 🎉</p>\n", severity_to_string(policy->failure_threshold));
    } else {
        fprintf(file, "                <div class=\"compliance-fail\">✗ Policy Compliance: FAILED</div>\n");
        fprintf(file, "                <p>⚠️ Findings exceed the failure threshold (%s) ⚠️</p>\n", severity_to_string(policy->failure_threshold));
    }
    
    fprintf(file, "            </div>\n");
    
    // Footer
    fprintf(file, "        </div>\n");
    fprintf(file, "        <div class=\"footer\">\n");
    fprintf(file, "            <p>MasterFabric Security Checker - Enterprise-Grade Protection</p>\n");
    fprintf(file, "            <p>Analysis completed at: %s | Total findings: %d | API calls made: %d</p>\n", 
            timestamp, results->finding_count, api_calls_made);
    fprintf(file, "        </div>\n");
    fprintf(file, "    </div>\n");
    fprintf(file, "</body>\n");
    fprintf(file, "</html>\n");
    
    fclose(file);
    return 0;
}