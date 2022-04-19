//
// Created by Tobiathan on 9/30/21.
//

#ifndef SENIORRESEARCH_RENDERTARGET_H
#define SENIORRESEARCH_RENDERTARGET_H

#include <glad.h>
#include "../util/Util.h"

class RenderTarget {
public:
    RenderTarget(GLint width, GLint height, bool hasDepth = false);
    ~RenderTarget();

    static void Bind(const RenderTarget& renderTarget);
    static void Unbind();

    static std::vector<unsigned char> SampleCentralSquare(const RenderTarget& renderTarget, int sampleCount);

    [[nodiscard]] GLuint GetTexture() const { return textureID; }

    void ChangeDimensions(GLint width, GLint height);
    void ChangeDimensions(Vec2 vec) { ChangeDimensions((GLint) vec.x, (GLint) vec.y); }
private:
    GLuint fboID, rboID, textureID;
    GLint width, height;
    bool hasDepth;
};


#endif //SENIORRESEARCH_RENDERTARGET_H
