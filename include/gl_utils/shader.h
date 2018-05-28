#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

/**
 * Define a macro that simplify the
 *
 * @param program: The program id
 * @param name: the name of the uniform to change
 * @param type: the type of the uniform to change (4f, ...)
 * @param value: the value to pass to glUniform
 *               (Warning, nbr of arg can change depending of type)
 */
#define UNIFORM_SET(program, name, type, ...)\
    glUniform ## type(glGetUniformLocation((program), (name)), __VA_ARGS__)

/**
 * A struct that contains information to contruct a shader from a file
 * It is used to pass informations to create_program_from_shader_files
 */
struct shader_file {
    GLenum shader_type;
    char* filename;
};

/**
 * Create a program from a list of shader files
 *
 * Don't forget to delete the program using glDeleteProgram
 *
 * @param shaders: an array of shader_file that contains the shaders sources
 * @param length: The lenght of the shaders array
 */
unsigned int create_program_from_shader_files(struct shader_file* shaders, size_t length);

/**
 * Link a list of shader into a program
 *
 * Don't forget to delete the program using glDeleteProgram
 *
 * @param shaders: a list of shaders ids
 * @param length: the length of the shader list
 * @return: the program id if everything is OK else 0
 */
unsigned int link_shaders(const unsigned int* shaders, size_t length);

/**
 * Compile a shader from a filename.
 *
 * Don't forget to delete the shader using glDeleteShader
 *
 * This function read the file with the filename specified and compile it.
 * @return the shader id if everything is OK else 0
 */
unsigned int compile_shader_from_file(const char* filename, GLenum shader_type);

/**
 * Compile a shader from a buffer
 *
 * Don't forget to delete the shader using glDeleteShader
 *
 * @param source: A buffer with contains the shader source
 * @param shader_type: The type of shader (vertex, frag, ...)
 * @return: the shader id if everything is OK else 0
 */
unsigned int compile_shader(const char* source, GLenum shader_type);

#endif /* ifndef SHADER_H */
