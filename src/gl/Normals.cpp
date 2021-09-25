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

        //printf("%i, %i, %i\n", row, numOfIndices, numOfIndices / 3);

        const GLuint start = row * 3;

        const GLuint p1Idx = indices[start] * 6;
        const GLuint p2Idx = indices[start + 1] * 6;
        const GLuint p3Idx = indices[start + 2] * 6;

        const glm::vec3 p1{arr[p1Idx], arr[p1Idx + 1], arr[p1Idx + 2]};
        const glm::vec3 p2{arr[p2Idx], arr[p2Idx + 1], arr[p2Idx + 2]};
        const glm::vec3 p3{arr[p3Idx], arr[p3Idx + 1], arr[p3Idx + 2]};

        const glm::vec3 normCross = glm::normalize(glm::cross((p2 - p1), (p3 - p1)));

        const auto func = [&](GLuint i) {
            arr[i + 3] += normCross.x;
            arr[i + 4] += normCross.y;
            arr[i + 5] += normCross.z;
        };
        func(p1Idx);
        func(p2Idx);
        func(p3Idx);
    }

//    for (size_t i = 0; i < count / 6; i++) {
//        const GLuint s = i * 6;
//        printf("%f, %f, %f :: %f, %f, %f\n", arr[s], arr[s + 1], arr[s + 2], arr[s + 3], arr[s + 4], arr[s + 5]);
//    }

    return {arr, arr + sizeof(arr) / sizeof(GLfloat)};
}
