#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "yaml_parser.h"
#include "sdk_analyzer.h"

// Enhanced reporting functions with animations and colors
void print_header();
void print_loading_animation(const char* message);
void print_progress_bar(int current, int total, const char* label);
void print_section_header(const char* title, const char* icon);
void print_cve_api_status(int api_calls, int vulnerabilities_found);

// Original reporting functions (enhanced)
void print_policy_summary(const security_policy_t* policy);
void print_finding_summary(const scan_results_t* results);
void print_detailed_findings(const scan_results_t* results, int verbose);
void print_recommendations(const scan_results_t* results);
void print_compliance_status(const scan_results_t* results, const security_policy_t* policy);
void print_footer(const scan_results_t* results, int api_calls_made);
void generate_security_report(const scan_results_t* results, const security_policy_t* policy, int verbose, int api_calls_made);

// HTML export function
int generate_html_report(const scan_results_t* results, const security_policy_t* policy, const char* output_file, int api_calls_made);

// Legacy functions (for compatibility)
void generate_report(const scan_results_t* results, const security_policy_t* policy, int verbose);
int determine_exit_code(const scan_results_t* results, const security_policy_t* policy);

// Helper functions
const char* repeat_char(const char* str, int count);

#endif // REPORT_GENERATOR_H
