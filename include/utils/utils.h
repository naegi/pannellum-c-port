#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"

/**
 * An implem of strdup because no standard C so don't exist as it
 */
static inline char* strdup(const char* s){
    size_t len = strlen(s) + 1;
    char* ret = malloc(len);

    return memcpy(ret, s, len);
}

/**
 * Read a file and put all its content into a buffer
 *
 * @param filename the filename of the file to read
 * @return a ptr to a buffer, NULL if any error. Need to be freed by caller
 */
static inline char* file_open_safe_into_buffer(const char* filename){
    FILE* fp = fopen(filename, "r");
    if(!fp){
        error("Can't open json file %s\n", filename);
        return NULL;
    }

    // get file size
    long int file_size;
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    // Read the file
    char* buffer = (char*) malloc (sizeof(char) * (file_size + 1));
    if (buffer == NULL) {
        error("Can't allocate buffer of json file's size "
              "(file %s of size %ld)\n", filename, file_size);
        fclose(fp);
        return NULL;
    }

    size_t result = fread(buffer, sizeof(char), file_size, fp);
    if (result != file_size) {
        error_with_errno("Error while reading file %s\n", filename);
        fclose(fp);
        return NULL;
    }

    buffer[file_size] = '\0';
    fclose(fp);

    return buffer;
}

/**
 * Safe function to load integer into a string
 *
 * @param integer The integer to load into a string
 * @param a ptr to the ptr to set (&a with typeof(a) == typeof(char *))
 * @return The length of the string written into *s
 *         the ptr passed contain data allocated by malloc. Caller have to free it.
 */
static inline size_t int_to_str(int integer, char** s){
    size_t len = snprintf(NULL, 0, "%d", integer);
    *s = malloc(len + 1);
    snprintf(*s, len + 1, "%d", integer);
    return len;
}

#endif /* ifndef UTILS_H */
