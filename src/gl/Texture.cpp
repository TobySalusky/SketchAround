//
// Created by Tobiathan on 9/22/21.
//

#include <glew.h>
#include "Texture.h"
#include "../vendor/stb/stb_image.h"
#include "../util/Util.h"

Texture::Texture(const char* path) {

    //stbi_set_flip_vertically_on_load(1); // correct image for opengl formatting

    localBuffer = stbi_load(path, &width, &height, &bitsPerPixel, 4);

    if (!localBuffer) {
	    LOG("Failed to find: %s\n", path);
        return;
    }

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linear is smoothing
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

Texture::Texture(int width, int height, std::vector<unsigned char> rgbData) {

    assert(width * height * 3 == rgbData.size());

    this->width = width;
    this->height = height;

    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Set(int width, int height, std::vector<unsigned char> rgbData) {
    assert(width * height * 3 == rgbData.size());

    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbData.data());
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Texture::Set(int width, int height, unsigned char* rgbDataPtr) {
    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbDataPtr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

