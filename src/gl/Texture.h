//
// Created by Tobiathan on 9/22/21.
//

#ifndef SENIORRESEARCH_TEXTURE_H
#define SENIORRESEARCH_TEXTURE_H


#include <string>

class Texture {
public:
    unsigned int ID;

    Texture(const char *path);
    ~Texture();

    void Bind(unsigned int slot = 0);
    void Unbind();

private:
    unsigned char* localBuffer;
    int width, height, bitsPerPixel;
};


#endif //SENIORRESEARCH_TEXTURE_H
