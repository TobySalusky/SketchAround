//
// Created by Tobiathan on 9/18/21.
//

#include "Mesh.h"
#include "Normals.h"
#include "../generation/Intersector.h"
#include <string>


Mesh::Mesh() : Mesh(nullptr, nullptr, 0, 0) {}

Mesh::Mesh(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) {
    VBO = 0;
    VAO = 0;
    IBO = 0;
    indexCount = 0;
    Init(vertices, indices, numOfVertices, numOfIndices);
}

Mesh::~Mesh() {
    ClearMesh();
}

void Mesh::Init(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) {
    indexCount = numOfIndices;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, nullptr, usageHint);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, nullptr, usageHint);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // must unbind after VAO

    Set(vertices, indices, numOfVertices, numOfIndices);
}

void Mesh::Render() const {

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::ClearMesh() {

    if (IBO != 0) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}

void Mesh::Set(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) {

    const std::vector<GLfloat> data = Normals::Define(vertices, indices, numOfVertices, numOfIndices);

    indexCount = numOfIndices;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numOfIndices, indices, usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), &data[0], usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Set(const std::vector<glm::vec3> &vertices, const std::vector<GLuint> &indices) {
    Set((GLfloat*) &vertices[0], (GLuint*) &indices[0], vertices.size() * 3, indices.size());
}

void Mesh::Set(const std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> &tuple) {
    const auto& [vertices, indices] = tuple;
    Set(vertices, indices);
}

std::string Mesh::GenOBJ(const std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> &tuple) {
    const auto& [vertices, indices] = tuple;
    return GenOBJ(vertices, indices);
}

std::string Mesh::GenOBJ(const std::vector<glm::vec3> &vertices, const std::vector<GLuint> &indices) {
    std::string vertStr, /*normalStr,*/ faceStr;

    const std::vector<GLfloat> data = Normals::Define(vertices, indices);

    for (int i = 0; i < data.size(); i += 6) {
        vertStr += "v " + std::to_string(data[i + 0]) + " " + std::to_string(data[i + 1]) + " " + std::to_string(data[i + 2]) + "\n";
        //normalStr += "vn " + std::to_string(data[i + 3]) + " " + std::to_string(data[i + 4]) + " " + std::to_string(data[i + 5]) + "\n";
    }

    for (int i = 0; i < indices.size(); i += 3) {
        faceStr += "f " + std::to_string(indices[i + 0] + 1) + " " + std::to_string(indices[i + 1] + 1) + " " + std::to_string(indices[i + 2] + 1) + "\n";
    }

    return vertStr + /*"\n" + normalStr +*/ "\n" + faceStr;
}

std::optional<MeshIntersection> Mesh::Intersect(const std::tuple<Vec3List, std::vector<GLuint>> &tuple, glm::mat4 modelMat, Ray ray) {

    const auto &[vertices, indices] = tuple;

    bool hasHit = false;

    MeshIntersection nearestIntersection {};
    MeshIntersection newIntersection {};

//    {
//        glm::mat4 inverseModelMat = glm::inverse(modelMat);
//        ray.origin = inverseModelMat * Vec4(ray.origin, 1.0f);
//        ray.dir = inverseModelMat * Vec4(ray.dir, 0.0f);
//    }

    for (int i = 0; i < indices.size(); i += 3) {
//        const Vec3 p1 = vertices[indices[i]];
//        const Vec3 p2 = vertices[indices[i + 1]];
//        const Vec3 p3 = vertices[indices[i + 2]];
        const Vec3 p1 = modelMat * Vec4(vertices[indices[i]], 1.0f);
        const Vec3 p2 = modelMat * Vec4(vertices[indices[i + 1]], 1.0f);
        const Vec3 p3 = modelMat * Vec4(vertices[indices[i + 2]], 1.0f);

        bool hit = Intersector::RayTriangleIntersection(ray, p1, p2, p3, newIntersection);

        if (hit) {
            if (!hasHit || glm::length(newIntersection.pos - ray.origin) < glm::length(nearestIntersection.pos - ray.origin)) {
                nearestIntersection = newIntersection;
            }

            hasHit = true;
        }
    }

    if (hasHit) return {nearestIntersection};
    return std::nullopt;
}