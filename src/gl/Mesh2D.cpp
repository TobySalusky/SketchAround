//
// Created by Tobiathan on 9/24/21.
//

#include "Mesh2D.h"

Mesh2D::Mesh2D() {
    VBO = 0;
    VAO = 0;
    IBO = 0;
    indexCount = 0;
    Init();
}

Mesh2D::~Mesh2D() {
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

void Mesh2D::Init() {

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, usageHint); // SUS

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, usageHint); // SUS

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // must unbind after VAO
}

void Mesh2D::Render() const {

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh2D::ImmediateRender() {
    Set((GLfloat*)&vertices[0], &indices[0], vertices.size() * 2, indices.size());
    Render();
}

void Mesh2D::ClearingRender() {
    Render();
    Clear();
}

void Mesh2D::ImmediateClearingRender() {
    ImmediateRender();
    Clear();
}

void Mesh2D::Clear() {
    vertices.clear();
    indices.clear();
}

// make args const?
void Mesh2D::Set(GLfloat *vertices, GLuint *indices, GLuint numOfVertices, GLuint numOfIndices) {
    indexCount = numOfIndices;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numOfIndices, indices, usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numOfVertices, vertices, usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh2D::AddLines(const std::vector<glm::vec2> &points, float width) {

    for (int i = 0; i < points.size(); i++) {
        auto pos = points[i];

        float angle = 0.0f;
        if (i == points.size() - 1) {
            if (i != 0) {
                auto vec = pos - points[i - 1];
                angle = atan2(vec.y, vec.x);
            }
        } else {
            auto vec = points[i + 1] - pos;
            angle = atan2(vec.y, vec.x);
        }
        angle += M_PI / 2;

        const auto diff = glm::vec2(width * cos(angle), width * sin(angle));

        vertices.emplace_back(pos - diff);
        vertices.emplace_back(pos + diff);
        if (i > 0) {
            const GLuint p1 = (i -1) * 2;
            const GLuint p2 = p1 + 1;
            const GLuint p3 = p2 + 1;
            const GLuint p4 = p3 + 1;
            indices.insert(indices.end(), {
                    p1, p2, p3,
                    p2, p3, p4,
            });
        }
    }
}

void Mesh2D::AddQuad(glm::vec2 tl, glm::vec2 br) {
    vertices.insert(vertices.end(), {
            {0.0f, 0.0f},
            {0.5f, 0.0f},
            {0.0f, 0.5f},
            {0.5f, 0.5f},
    });
    const GLuint p1 = vertices.size();
    const GLuint p2 = p1 + 1;
    const GLuint p3 = p2 + 1;
    const GLuint p4 = p3 + 1;
    indices.insert(indices.end(), {
            p1, p2, p3,
            p2, p3, p4,
    });
}
