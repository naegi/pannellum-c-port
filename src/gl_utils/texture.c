#include <stdbool.h>

#include "glad/glad.h"

#include "utils/stb_image.h"
#include "utils/log.h"

bool load_img_into_texture(char* filename, bool generate_mipmaps){
    info("Loading texture under filename %s\n", filename);
    int width, height, components_per_pixels;

    unsigned char* data = stbi_load(filename , &width, &height, &components_per_pixels, 0);
    if(data == NULL){
        error("Can't load texture %s\n", filename);
        return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    switch (components_per_pixels) {
        // Read doc for Greyscale/Greyscale + alpha
        case 1: // Greyscale
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            break;
        case 2: // Grey + alpha
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
            break;
        case 3: // RGB
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        case 4: // RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width , height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        default:
            error("Unknow number of components per pixel (%d) in filename %s\n",
                    components_per_pixels, filename);
            return false;

    }

    stbi_image_free(data);
    info("Texture loaded\n");

    if(generate_mipmaps){
        info("Generating mipmaps for file %s\n", filename);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

bool load_imgs_into_textures(char** filenames, unsigned int* texture_ids, size_t length, bool generate_mipmaps){
    info("Starting to load bunch of textures\n");
    bool success = true;
    for (int i = 0; i < length; ++i) {
        glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
        success &= load_img_into_texture(filenames[i], generate_mipmaps);
    }

    if(success){
        info("Bunch of textures loaded\n");
    }else{
        error("Can't load a bunch of texture ;-(\n");
    }
    return success;
}
