//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_MESH_H
#define SENIORRESEARCH_MESH_H

#include <glew.h>


class Mesh {
public:

    GLenum usageHint = GL_DYNAMIC_DRAW;

    Mesh();
    Mesh(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    ~Mesh();

    void Init(GLfloat* vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
    void Set(GLfloat* vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) const;
    void Render() const;
    void ClearMesh();
private:
    GLuint VAO, VBO, IBO, indexCount;
};


#endif //SENIORRESEARCH_MESH_H
