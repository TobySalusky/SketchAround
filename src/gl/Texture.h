//
// Created by Tobiathan on 9/22/21.
//

#ifndef SENIORRESEARCH_TEXTURE_H
#define SENIORRESEARCH_TEXTURE_H


#include <string>
#include <vector>

class Texture {
public:
    unsigned int ID;

    explicit Texture(const char *path);
    Texture(int width, int height, std::vector<unsigned char> rgbData); // three unsigned bytes per pixel
    ~Texture();

    void Bind(unsigned int slot = 0);
    void Unbind();

    void Set(int width, int height, std::vector<unsigned char> rgbData);
    void Set(int width, int height, unsigned char* rgbDataPtr);

private:
    unsigned char* localBuffer;
    int width, height, bitsPerPixel;
};


#endif //SENIORRESEARCH_TEXTURE_H
