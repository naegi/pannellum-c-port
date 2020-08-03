#include "cjson/cJSON.h"

#include "utils/json_parse.h"
#include "utils/args.h"
#include "utils/utils.h"

#include "config.h"

#define copy_json_name_object(name) __str_##name
#define copy_json_str(json, name, into) {\
    cJSON* copy_json_name_object(name) = cJSON_GetObjectItemCaseSensitive((json), (#name));\
    if(cJSON_IsString(copy_json_name_object(name)) && \
            copy_json_name_object(name)->valuestring != NULL){\
        (into) = strdup(copy_json_name_object(name)->valuestring);\
    }\
}

#define copy_json_int(json, name, into) {\
    cJSON* copy_json_name_object(name) = cJSON_GetObjectItemCaseSensitive((json), (#name));\
    if(cJSON_IsNumber(copy_json_name_object(name))){\
        (into) = copy_json_name_object(name)->valueint;\
    }\
}

void* json_parse_multires(cJSON* json){
    config_multires_t* a = malloc(sizeof(config_multires_t));

    cJSON* multires = cJSON_GetObjectItemCaseSensitive(json, "multiRes");
    copy_json_str(multires, path, a->path_format);

    copy_json_int(multires, tileResolution, a->tile_size);
    copy_json_int(multires, cubeResolution, a->cube_size);
    copy_json_int(multires, maxLevel, a->max_level);

    return a;
}

config_elem_t* json_parse_elem_config(char* filename){
    info("Parsing elem config JSON %s\n", filename);


    config_elem_t* a = malloc(sizeof(config_elem_t)) ;

    char* buffer = file_open_safe_into_buffer(filename);
    if(buffer == NULL)
        return NULL;

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            error("Error while reading JSON: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        free(buffer);
        free(a);
        return NULL;
    }

    char* renderer = NULL;
    copy_json_str(json, type, renderer);
    a->type = renderer_str_to_enum(renderer);

    switch (a->type) {
        case CONFIG_MULTIRES:
            a->config = json_parse_multires(json);
            break;
        case CONFIG_EQUIRECTANGULAR:
        case CONFIG_CUBEMAP:
            error("raise NotImplementedError\n");
            exit(-1);
        default:
            error("Renderer type in %s has not been recognized\n", filename);
            help("Pannellum");
            exit(-1);
    }
    cJSON_Delete(json);
    free(buffer);

    return a;
}


config_t* json_parse_main_config(char* filename){
    info("Parsing main config JSON %s\n", filename);

    config_t* a = malloc(sizeof(config_t)) ;

    char* buffer = file_open_safe_into_buffer(filename);
    if(buffer == NULL)
        return NULL;

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            error("Error while reading JSON: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        free(buffer);
        free(a);
        return NULL;
    }

    copy_json_str(json, config_path, a->config_path);
    copy_json_str(json, base_path, a->base_path);
    copy_json_int(json, start, a->start);


    cJSON* array = cJSON_GetObjectItem(json, "valid_ids");
    cJSON* id = NULL;
    if(!cJSON_IsArray(array)){
        error("valid_ids should be an array\n");
        exit(-1);
    }

    a->valid_ids_length = cJSON_GetArraySize(array);
    a->valid_ids = malloc(sizeof(int) * a->valid_ids_length);
    int j = 0;
    cJSON_ArrayForEach(id, array){
        if(!cJSON_IsNumber(id)){
            error("ids in valid_ids shoulds be numbers");
            exit(-1);
        }
        a->valid_ids[j] = id->valueint;
        j++;
    }

    array = cJSON_GetObjectItem(json, "path");
    if(!cJSON_IsArray(array)){
        error("path should be an array\n");
        exit(-1);
    }

    a->links_length = cJSON_GetArraySize(array);
    warn("Links are not implemented\n");

    cJSON_Delete(json);
    free(buffer);

    return a;
}
#undef copy_json_str_name_object
#undef copy_json_int
#undef copy_json_str
