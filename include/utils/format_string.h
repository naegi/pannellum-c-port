#ifndef FORMAT_STR_H
#define FORMAT_STR_H
void format_string_img_multires(char* restrict dest, size_t max_size,
        const char* restrict format, const char* restrict base_path, int face,
        int x, int y, int level);

void format_string_config(char* restrict dest, size_t max_size,
        const char* restrict base_path, const char* restrict config_path, int id);
#endif /* ifndef FORMAT_STR_H */
