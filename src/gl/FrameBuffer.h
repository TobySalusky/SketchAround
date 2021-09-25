//
// Created by Tobiathan on 9/22/21.
//

#ifndef SENIORRESEARCH_FRAMEBUFFER_H
#define SENIORRESEARCH_FRAMEBUFFER_H


class FrameBuffer {
public:

    void CreateBuffers(int width, int height);

    void DeleteBuffers();

    void Bind();

    void Unbind();

    unsigned int GetTexture();

private:
    unsigned int textureID = 0;
    unsigned int fboID = 0;
    unsigned int depthID = 0;
    int width = 0;
    int height = 0;
};


#endif //SENIORRESEARCH_FRAMEBUFFER_H
