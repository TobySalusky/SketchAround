//
// Created by Tobiathan on 9/21/21.
//

#ifndef SENIORRESEARCH_NORMALS_H
#define SENIORRESEARCH_NORMALS_H

#include <glew.h>
#include <vector>
#include "vendor/glm/vec3.hpp"

class Normals {

public:

    static std::vector<GLfloat> Define(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices) {
        return Define((GLfloat*) &vertices[0], &indices[0], vertices.size() * 3, indices.size());
    }

    static std::vector<GLfloat> Define(const GLfloat *vertices, const GLuint *indices, GLuint numOfVertices, GLuint numOfIndices);
};


#endif //SENIORRESEARCH_NORMALS_H
