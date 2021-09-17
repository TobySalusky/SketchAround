//
// Created by Tobiathan on 9/13/21.
//

#include <iostream>
#include <glew.h>
#include <vector>

#include "VBO.h"

VBO::VBO() { // NOLINT(cppcoreguidelines-pro-type-member-init)
    glGenBuffers(1, &ID);
}

VBO::~VBO() {
    glDeleteBuffers( 1, &ID );
    std::cout << "VBO is gone" << '\n';
}

void VBO::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::UnBind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::BufferData(std::vector<GLfloat> vec) {
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), &vec[0], GL_DYNAMIC_DRAW);
    UnBind();
}