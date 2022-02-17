//
// Created by Tobiathan on 9/24/21.
//

#include "Mesh2D.h"
#include "../util/Util.h"

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



    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*) (2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);



    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // must unbind after VAO
}

void Mesh2D::Render() const {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr); // FIXME: narrowing?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
}

void Mesh2D::ImmediateRender() {
    Set((GLfloat*)&vertexData[0], &indices[0], vertexData.size(), indices.size());
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
    vertexData.clear();
    indices.clear();
}

// make args const?
void Mesh2D::Set(GLfloat *vertexDataArr, GLuint *indices, GLuint vertexDataCount, GLuint numOfIndices) {
    indexCount = numOfIndices;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numOfIndices, indices, usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexDataCount, vertexDataArr, usageHint);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh2D::AddLines(const std::vector<glm::vec2> &points, glm::vec4 color, float width) {

    width /= scaleLines;

    const GLuint startIndex = vertexData.size() / 6;

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

        AddVertex(pos - diff, color);
        AddVertex(pos + diff, color);
        if (i > 0) {
            const GLuint p1 = (i -1) * 2 + startIndex;
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

void Mesh2D::AddQuad(glm::vec2 corner1, glm::vec2 corner2, glm::vec4 color) {
    const GLuint p1 = vertexData.size() / 6;
    const GLuint p2 = p1 + 1;
    const GLuint p3 = p2 + 1;
    const GLuint p4 = p3 + 1;

    AddVertex(corner1, color);
    AddVertex({corner1.x, corner2.y}, color);
    AddVertex({corner2.x, corner1.y}, color);
    AddVertex(corner2, color);
    /*vertexData.insert(vertexData.end(), {
            corner1.x, corner1.y, color.r, color.g, color.b, color.a,
            corner1.x, corner2.y, color.r, color.g, color.b, color.a,
            corner2.x, corner1.y, color.r, color.g, color.b, color.a,
            corner2.x, corner2.y, color.r, color.g, color.b, color.a,
    });*/
    indices.insert(indices.end(), {
            p1, p2, p3,
            p2, p3, p4,
    });
}

void Mesh2D::AddPolygonOutline(glm::vec2 center, float rad, int pointCount, glm::vec4 color, float width) {
    rad /= scaleLines;

    std::vector<glm::vec2> points;

    for (int i = 0; i <= pointCount; i++) {
        float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) pointCount));
        points.emplace_back(center + Util::Polar(rad, angle));
    }

    AddLines(points, color, width);
}
