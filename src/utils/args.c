#include "utils/args.h"

#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "config.h"

int renderer_str_to_enum(char* renderer_str){
    if (strcmp(renderer_str, "multires") == 0)
        return CONFIG_MULTIRES;
    else if (strcmp(renderer_str, "equirectangular") == 0)
        return CONFIG_EQUIRECTANGULAR;
    else if (strcmp(renderer_str, "cubemap") == 0)
        return CONFIG_CUBEMAP;
    return -1;
}

void parse_args(struct args* a, int argc, char **argv) {
    int c;
    static struct option long_options[] = {
        {  "help" , no_argument , 0 , 'h' } ,
        { "debug" , no_argument , 0 , 'd' } ,
        {    0    ,      0      , 0 ,  0 }
    };

    while (1) {
        int option_index = 0;

        c = getopt_long(argc, argv, "hd", long_options, &option_index);
        if (c == -1)
            break;
        switch (c) {
            case 'd':
                a->debug = true;
                break;
            case 'h':
                help(argv[0]);
                exit(0);
                break;
            default:
                help(argv[0]);
                exit(-1);
        }
    }

    if (optind < argc){
        a->json = argv[optind];
    }
    else{
        help(argv[0]);
        exit(-1);
    }
}

void help(char* program_name){
    printf(
            "Usage: %s [OPTION...] [JSON_FILE]\n"
#ifndef DEBUG
            "\t -d, --debug\t\t\t\tSetup OpenGL is debug mode\n"
#else
            "\t -d, --debug\t\t\t\tTarget is already DEBUG, will do nothing\n"
#endif
            "\t -h, --help\t\t\t\tPrint this message\n"
            "", program_name);
}
