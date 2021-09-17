//
// Created by Tobiathan on 9/13/21.
//

#ifndef TEST_VBO_H
#define TEST_VBO_H


#include "IBind.h"

class VBO : public IBind {
public:
    GLuint ID;

    VBO();
    ~VBO() override;

    void BufferData(std::vector<GLfloat> vec);

    void Bind() override;
    void UnBind() override;
};

#endif //TEST_VBO_H
