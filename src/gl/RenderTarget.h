//
// Created by Tobiathan on 9/30/21.
//

#ifndef SENIORRESEARCH_RENDERTARGET_H
#define SENIORRESEARCH_RENDERTARGET_H

#include <glew.h>

class RenderTarget {
public:
    RenderTarget(GLint width, GLint height, bool hasDepth = false);

    void Bind() const;
    static void Unbind();

    [[nodiscard]] GLuint GetTexture() const { return textureID; }
private:
    GLuint fboID, rboID, textureID;
    bool hasDepth;
};


#endif //SENIORRESEARCH_RENDERTARGET_H
