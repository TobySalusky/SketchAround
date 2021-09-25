//
// Created by Tobiathan on 9/22/21.
//

#include "FrameBuffer.h"

#include <glew.h>
#include <cstdio>

void FrameBuffer::CreateBuffers(int width, int height) {
    this->width = width;
    this->width = height;

    if (fboID)
    {
        DeleteBuffers();
    }

    printf("eee");

    glGenFramebuffers(1, &fboID);    printf("eee");

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);    printf("eee");

    glGenTextures(1, &textureID);    printf("eee");

    glBindTexture(GL_TEXTURE_2D, textureID);

    printf("eee");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    printf("eee");

    glGenTextures(1, &depthID);
    glBindTexture(GL_TEXTURE_2D, depthID);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    printf("eee");


    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);

    GLenum buffers[4] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(textureID, buffers);

    printf("eee");


    Unbind();
}

void FrameBuffer::DeleteBuffers() {
    if (fboID)
    {
        glDeleteFramebuffers(GL_FRAMEBUFFER, &fboID);
        glDeleteTextures(1, &textureID);
        glDeleteTextures(1, &depthID);
        textureID = 0;
        depthID = 0;
    }
}

void FrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int FrameBuffer::GetTexture() {
    return textureID;
}
