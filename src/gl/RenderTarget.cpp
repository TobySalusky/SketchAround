//
// Created by Tobiathan on 9/30/21.
//

#include <cstdio>
#include <vector>
#include <algorithm>
#include "RenderTarget.h"

RenderTarget::RenderTarget(GLint width, GLint height, bool hasDepth) : hasDepth(hasDepth), width(width), height(height) { // NOLINT(cppcoreguidelines-pro-type-member-init)

    glGenFramebuffers(1, &fboID);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenRenderbuffers(1, &rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	    LOG("problem setting up framebuffer");
    }
}

void RenderTarget::Unbind() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void RenderTarget::Bind(const RenderTarget &renderTarget) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderTarget.fboID);
    if (renderTarget.hasDepth) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

std::vector<unsigned char> RenderTarget::SampleCentralSquare(const RenderTarget& renderTarget, int sampleCount) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget.fboID);

    const int dimens = std::min(renderTarget.width, renderTarget.height);
    unsigned char pixelBuff[dimens * dimens * 3];
    glReadPixels(std::max(renderTarget.width - renderTarget.height, 0) / 2, std::max(renderTarget.height - renderTarget.width, 0) / 2,
                 dimens, dimens, GL_RGB, GL_UNSIGNED_BYTE, pixelBuff);

    std::vector<unsigned char> sampled;

    const float step = (float) dimens / (float) sampleCount;
    for (int j = 0; j < sampleCount; j++) {
        for (int i = 0; i < sampleCount; i++) {
            const int xPixel = (int) ((float) i * step), yPixel = (int) ((float) j * step);
            const int index = (xPixel + yPixel * dimens) * 3;
            sampled.emplace_back(pixelBuff[index]);
            sampled.emplace_back(pixelBuff[index + 1]);
            sampled.emplace_back(pixelBuff[index + 2]);
        }
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return sampled;
}

RenderTarget::~RenderTarget() {
    LOG("Disposing render target");
    glDeleteTextures(1, &textureID);
    glDeleteFramebuffers(1, &fboID);
    glDeleteRenderbuffers(1, &rboID);
}

void RenderTarget::ChangeDimensions(GLint width, GLint height) {
    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindRenderbuffer(GL_RENDERBUFFER, rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	    LOG("problem resizing framebuffer");
    }
}
