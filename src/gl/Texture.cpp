//
// Created by Tobiathan on 9/22/21.
//

#include <glew.h>
#include "Texture.h"
#include "../vendor/stb/stb_image.h"

Texture::Texture(const char* path) {

    //stbi_set_flip_vertically_on_load(1); // correct image for opengl formatting

    localBuffer = stbi_load(path, &width, &height, &bitsPerPixel, 4);

    if (!localBuffer) {
        printf("Failed to find: %s\n", path);
        return;
    }

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    // free local buffer
    stbi_image_free(localBuffer);
}

Texture::~Texture() {
    glDeleteTextures(1, &ID);
}

void Texture::Bind(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

