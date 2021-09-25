//
// Created by Tobiathan on 9/24/21.
//

#ifndef SENIORRESEARCH_MESH2D_H
#define SENIORRESEARCH_MESH2D_H

#include <glew.h>
#include <vector>
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtx/vector_angle.hpp"


class Mesh2D {
public:

    GLenum usageHint = GL_DYNAMIC_DRAW;

    Mesh2D();
    ~Mesh2D();

    void Init();
    void Set(GLfloat* vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    void Render() const;
    void Clear();
    void ClearingRender();
    void AddLines(const std::vector<glm::vec2> &points, float width);
    void AddQuad(glm::vec2 corner1, glm::vec2 corner2);

    void ImmediateClearingRender();
    void ImmediateRender();

private:
    GLuint VAO, VBO, IBO, indexCount;
    std::vector<glm::vec2> vertices;
    std::vector<GLuint> indices;

};


#endif //SENIORRESEARCH_MESH2D_H
