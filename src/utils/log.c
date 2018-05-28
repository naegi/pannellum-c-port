#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include <stdlib.h>

#define LOG_DEFINE
#include "utils/log.h"

static void vlogsf(unsigned int log_level, FILE *f, const char* fmt, va_list args){
    // We assume log_level is correct ! Make sure it is.

    // is log level high enough to allow further processing ?
    if(log_level > LOG_LEVEL)
        return;

    fputs(log_colors_init[log_level], f);
    vfprintf(f, fmt, args);
    fputs(log_colors_end, f);


}

void logsf(unsigned int log_level, FILE *f, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    vlogsf(log_level, f, fmt, args);

    va_end(args);
}


void error_with_errno(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    char* err_str = strerror(errno);

    char* buffer = malloc(sizeof(char) * (strlen(err_str) + strlen(fmt) + 1));
    if(buffer == NULL){
        error("BRRRRUUUUHHH C'EST LA MERDE!"
              "ÇA MERDE DURANT UNE ERREEUUR J'ME CASSE\n");
        exit(':' + '\'' +'('); // PLEEEEURRREEEUUUH
    }

    strcpy(buffer, fmt);
    strcat(buffer, err_str);

    vlogsf(LOG_ERROR, stderr, buffer, args);

    free(buffer);

    va_end(args);
}
