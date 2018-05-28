#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"

#include "utils/utils.h"
#include "utils/log.h"
#include "gl_utils/shader.h"

unsigned int compile_shader_from_file(const char* filename, GLenum shader_type){
    info("Compiling shader from file %s\n", filename);

    char* shader_source = file_open_safe_into_buffer(filename);
    if(shader_source == NULL)
        return 0;

    unsigned int shader_id = compile_shader(shader_source, shader_type);
    free(shader_source);

    if(shader_id == 0){
        error("Can't compile shader from file %s\n", filename);
        return 0;
    }
    info("Shader compiled from file %s! (Without error)\n", filename);
    return shader_id;
}

unsigned int compile_shader(const char* source, GLenum shader_type){
    unsigned int shader_id;
    shader_id = glCreateShader(shader_type);

    glShaderSource(shader_id, 1, &source, NULL);
    glCompileShader(shader_id);


    int success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if(!success){
        int log_size;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);

        char* buffer_log = (char*) malloc (sizeof(char) * log_size);
        glGetShaderInfoLog(shader_id, log_size, NULL, buffer_log);
        error("Can't compile shader. Shader info logs: \n%s\n", buffer_log);

        free(buffer_log);
        return 0;
    }

    return shader_id;
}

unsigned int link_shaders(const unsigned int* shaders, size_t length){
    info("Linking a list of shaders\n");

    unsigned int program_id = glCreateProgram();

    for (int i = 0; i < length; ++i) {
        glAttachShader(program_id, shaders[i]);
    }
    glLinkProgram(program_id);

    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);

    if(!success){
        int log_size;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_size);

        char* buffer_log = (char*) malloc (sizeof(char) * log_size);
        glGetProgramInfoLog(program_id, log_size, NULL, buffer_log);
        error("Can't link shaders. Shader info logs: \n%s\n", buffer_log);

        free(buffer_log);
        return 0;
    }

    info("The list of shaders is now linked\n");
    return program_id;
}

unsigned int create_program_from_shader_files(struct shader_file* shaders, size_t length){
    unsigned shader_ids[length];
    info("Creating Program from list of shaders files\n");

    for (int i = 0; i < length; ++i) {
        shader_ids[i] = compile_shader_from_file(shaders[i].filename, shaders[i].shader_type);
        if(shader_ids[i] == 0){
            error("Can't create program from shader list, a shader can't be created\n");
            return 0;
        }
    }

    unsigned int program_id = link_shaders(shader_ids, length);

    for (int i = 0; i < length; ++i) {
        glDeleteShader(shader_ids[i]);
    }

    info("Program is created from the list of shaders files\n");
    return program_id;
}

void send_uniform(unsigned int program_id, char* name, void* value){

}
