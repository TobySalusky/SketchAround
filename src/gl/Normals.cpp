//
// Created by Tobiathan on 9/21/21.
//

#include "Normals.h"
#include "../vendor/glm/geometric.hpp"

std::vector<GLfloat> Normals::Define(const GLfloat *vertices, const GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) {

    size_t count = numOfVertices * 2;
    GLfloat arr[count];

    // TODO: cleanup PLEASE

    // setup new structure
    for (size_t row = 0; row < numOfVertices / 3; row++) {
        const GLuint initStart = row * 3;
        const GLuint newStart = row * 6;
        arr[newStart] = vertices[initStart];
        arr[newStart + 1] = vertices[initStart + 1];
        arr[newStart + 2] = vertices[initStart + 2];
        arr[newStart + 3] = 0;
        arr[newStart + 4] = 0;
        arr[newStart + 5] = 0;
    }


    // add normals
    for (GLuint row = 0; row < numOfIndices / 3; row++) {

        const GLuint start = row * 3;

        const GLuint p1Idx = indices[start] * 6;
        const GLuint p2Idx = indices[start + 1] * 6;
        const GLuint p3Idx = indices[start + 2] * 6;

        const glm::vec3 p1{arr[p1Idx], arr[p1Idx + 1], arr[p1Idx + 2]};
        const glm::vec3 p2{arr[p2Idx], arr[p2Idx + 1], arr[p2Idx + 2]};
        const glm::vec3 p3{arr[p3Idx], arr[p3Idx + 1], arr[p3Idx + 2]};

        const glm::vec3 normCross = glm::normalize(glm::cross((p2 - p1), (p3 - p1)));

        const auto Func = [&](GLuint i) {
            arr[i + 3] += normCross.x;
            arr[i + 4] += normCross.y;
            arr[i + 5] += normCross.z;
        };
        Func(p1Idx);
        Func(p2Idx);
        Func(p3Idx);
    }


    return {arr, arr + sizeof(arr) / sizeof(GLfloat)};
}
