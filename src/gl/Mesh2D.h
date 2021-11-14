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
    void Set(GLfloat* vertexDataArr, GLuint *indices, GLuint vertexDataCount, GLuint numOfIndices);
    void Render() const;
    void Clear();
    void ClearingRender();
    void AddLines(const std::vector<glm::vec2> &points, glm::vec4 color, float width = 0.01f);
    void AddQuad(glm::vec2 corner1, glm::vec2 corner2, glm::vec4 color);
    void AddQuad(glm::vec2 corner1, glm::vec2 corner2, glm::vec3 color) {
        AddQuad(corner1, corner2, glm::vec4(color.r, color.g, color.b, 1.0f));
    }
    void AddPolygonOutline(glm::vec2 center, float rad, int pointCount, glm::vec4 color, float width = 0.01f);

    void ImmediateClearingRender();
    void ImmediateRender();

private:
    GLuint VAO, VBO, IBO, indexCount;
    std::vector<float> vertexData;
    std::vector<GLuint> indices;

    void AddVertex(glm::vec2 pos, glm::vec4 color) {
        vertexData.insert(vertexData.end(), {
                pos.x, pos.y, color.r, color.g, color.b, color.a
        });
    }

    void AddVertex(glm::vec2 pos, glm::vec3 color) {
        vertexData.insert(vertexData.end(), {
                pos.x, pos.y, color.r, color.g, color.b, 1.0f
        });
    }
};


#endif //SENIORRESEARCH_MESH2D_H
