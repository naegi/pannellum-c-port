#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#define LOG_NONE 0
#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4

#ifdef LOG_DEFINE // LOG_DEFINE is set in log.c only

char* log_colors_end = "\e[0m"; // Reset colors
char* log_colors_init[] = {
    "",
    "\e[0;31mError: ", // Error - red
    "\e[0;33mWarning: ", // Warn  - yellow
    "\e[0;32mInfo: ", // Info  - green
    "Debug: "          // Debug - none
};

#endif

#define error(...) logsf(LOG_ERROR, stderr, __VA_ARGS__)
#define warn(...)  logsf(LOG_WARN,  stderr, __VA_ARGS__)
#define info(...)  logsf(LOG_INFO,  stdout, __VA_ARGS__)
#define debug(...) logsf(LOG_DEBUG, stdout, __VA_ARGS__)

void logsf(unsigned int log_level, FILE *f, const char* fmt, ...);
void error_with_errno(const char* fmt, ...);

#endif /* ifndef LOG_H */
