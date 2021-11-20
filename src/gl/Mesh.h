//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_MESH_H
#define SENIORRESEARCH_MESH_H

#include <glew.h>
#include <vector>
#include "../vendor/glm/vec3.hpp"


class Mesh {
public:

    GLenum usageHint = GL_DYNAMIC_DRAW;

    Mesh();
    Mesh(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    ~Mesh();

    void Init(GLfloat* vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    void Set(GLfloat* vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    void Set(const std::tuple<std::vector<glm::vec3>, std::vector<GLuint>>& tuple);
    void Set(const std::vector<glm::vec3> &vertices, const std::vector<GLuint> &indices);
    void Render() const;
    void ClearMesh();

    static std::string GenOBJ(const std::vector<glm::vec3> &vertices, const std::vector<GLuint> &indices);
    static std::string GenOBJ(const std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> &tuple);

private:
    GLuint VAO, VBO, IBO, indexCount;
};


#endif //SENIORRESEARCH_MESH_H
