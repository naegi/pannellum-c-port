#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * Load an image into an already created GL_TEXTURE_2D
 *
 * NOTE: if the image is greyscale, only the red component will be set,
 *        you will need into the fragment shader to transform it as gray with something like
 *
 *        uniform sampler2D tex;
 *        vec3 result = vec4(texture(tex, TexCoord).rrr, 1.0);
 *
 *       if the image is in greyscale the red component and the green component will be set
 *        you will need to transform it in the frag shader with something like
 *
 *        uniform sampler2D tex;
 *        vec4 color = texture(tex, TexCoord).rrrg;
 *
 *  The texture should be binded using glTextureBind before this function is called
 *
 * @param filename the filename in which the texture is stored
 * @param do generate mipmap ?
 * @return true if everything is ok, false otherwise
 */
bool load_img_into_texture(char* filename, bool generate_mipmap);

/*
 * Load a bunch of images in already created GL_TEXTURE_2D
 *
 * NOTE: see load_img_into_texture
 *
 * Warning ! This function will bind textures !
 *
 * @param filenames an array of filenames in which are stored the textures datas
 * @param texture_id an array of texture_id in which we will store the texture data
 * @param length the length of filenames and texture_ids
 * @param do generate mipmap ?
 * @return true if everything is ok, false otherwise
 * */
bool load_imgs_into_textures(char** filenames, unsigned int* texture_ids, size_t length, bool generate_mipmap);

#endif /* ifndef TEXTURE_H */
