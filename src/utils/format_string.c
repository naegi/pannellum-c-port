#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "utils/format_string.h"
#include "utils/utils.h"

char *face_chars = "fbudlr";

void format_string_img_multires(char* restrict dest, size_t max_size, const char* restrict format,
        const char* restrict base_path, int face, int x, int y, int level){
    size_t size = 0;
    const char* p = format;
    char* d = dest;
    *d = 0;

    char* x_str;
    char* y_str;
    char* l_str;

    // Get size needed
    size_t lx, ly, ll, l_base_path;

    lx = int_to_str(x, &x_str);
    ly = int_to_str(y, &y_str);
    ll = int_to_str(level, &l_str);

#define check_size(amount)\
    size += (amount);\
    if(size > max_size){\
        *d = 0;\
        error("Format string overflowed allowed space !\n");\
        goto end; \
    }\

    l_base_path = strlen(base_path);
    check_size(l_base_path);
    memcpy(d, base_path, sizeof(char) * l_base_path);
    d += l_base_path;

    while(*p != '\0'){
        if(*p == '%'){
            p++;
            switch(*p) {
                case '%':
                    *d = '%';
                    d++;
                    *d = 0;
                    size += 1;
                    break;
                case 's':
                    *d = face_chars[face];
                    d++;
                    *d = 0;
                    size += 1;
                    break;
                case 'x':
                    check_size(lx)
                        strcat(d, x_str);
                    d += lx;
                    break;
                case 'y':
                    check_size(ly)
                        strcat(d, y_str);
                    d += ly;
                    break;
                case 'l':
                    check_size(ll)
                        strcat(d, l_str);
                    d += ll;
                    break;
            }
        }else{
            check_size(1)
                *d = *p;
            d++;
            *d = 0;
        }
        p++;
    }

end:
    free(l_str);
    free(y_str);
    free(x_str);
#undef check_size
}

void format_string_config(char* restrict dest, size_t max_size,
        const char* restrict base_path, const char* restrict config_path, int id){
    size_t size = 0;
    const char* p = base_path;
    char* d = dest;
    *d = 0;

    char* id_str;

    // Get size needed
    size_t l_id;

    l_id = int_to_str(id, &id_str);

#define check_size(amount)\
    size += (amount);\
    if(size > max_size){\
        *d = 0;\
        error("Format string overflowed allowed space !\n");\
        goto end; \
    }\

    while(*p != '\0'){
        if(*p == '%'){
            p++;
            switch(*p) {
                case '%':
                    *d = '%';
                    d++;
                    *d = 0;
                    size += 1;
                    break;
                case 'i':
                    check_size(l_id)
                        strcat(d, id_str);
                    d += l_id;
                    break;
            }
        }else{
            check_size(1)
                *d = *p;
            d++;
            *d = 0;
        }
        p++;
    }

    check_size(strlen(config_path));
    strcat(d, config_path);

end:
    free(id_str);
#undef check_size
}
