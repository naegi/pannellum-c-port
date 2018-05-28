#include "config.h"
#include "utils/utils.h"
#include "utils/json_parse.h"
#include "utils/format_string.h"

size_t configure_context(context_t* ctx, config_t* cfg){
    size_t start_id = 0;
    for (int i = 0; i < cfg->valid_ids_length; ++i) {
        size_t id = 0;
        char config_path[1024] = {0};

        format_string_config(config_path, 1024, cfg->base_path,
                cfg->config_path, cfg->valid_ids[i]);
        config_elem_t* cfg_elem = json_parse_elem_config(config_path);

        format_string_config(config_path, 1024, cfg->base_path,
                "", cfg->valid_ids[i]);

        switch (cfg_elem->type) {
            case CONFIG_MULTIRES:
                ((config_multires_t*)cfg_elem->config)->base_path = strdup(config_path);
                id = context_add_elem(ctx, CONFIG_MULTIRES, cfg_elem->config);
                break;
            case CONFIG_EQUIRECTANGULAR:
            case CONFIG_CUBEMAP:
                error("raise NotImplementedError\n");
                exit(-1);
        }
        free(cfg_elem->config);
        free(cfg_elem);

        if(cfg->valid_ids[i] == cfg->start)
            start_id = id;

    }
    return start_id;
}
