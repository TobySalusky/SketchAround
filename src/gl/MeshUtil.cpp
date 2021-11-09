//
// Created by Tobiathan on 11/8/21.
//

#include "MeshUtil.h"
#include "../vendor/glm/geometric.hpp"
#include "../vendor/glm/fwd.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> MeshUtil::Empty() {
    return {{}, {}};
}

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
MeshUtil::Quad3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4) {
    return {{p1, p2, p3, p4}, {0, 1, 2, 0, 2, 3}};
}

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
MeshUtil::Square(glm::vec3 midPoint, glm::vec3 dir, float sideLen) {
    glm::vec3 up {0.0f, 1.0f, 0.0f};
    glm::vec3 left = glm::normalize(glm::cross(up, dir));
    float halfSide = sideLen / 2.0f;
    return Quad3D(midPoint - halfSide * up - halfSide * left,
                  midPoint + halfSide * up - halfSide * left,
                  midPoint + halfSide * up + halfSide * left,
                  midPoint - halfSide * up + halfSide * left);
}

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
MeshUtil::PolyLine(const std::vector<glm::vec3>& points, float rad) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    const int countPerRing = 4;

    // TODO: factor in camera angle for billboard effect

    for (const auto &point : points) {

        for (int i = 0; i < countPerRing; i++) {
            float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) countPerRing));
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f,0.0f,0.0f)); // TODO: split mat creation from loop (b/c constant angles)
            glm::vec4 vec = rot * glm::vec4(0.0f, rad, 0.0f, 1.0f);

            vertices.emplace_back(glm::vec3(point.x, point.y + vec.y, point.z + vec.z));
        }
    }

    for (int ring = 0; ring < points.size() - 1; ring++) {

        bool reverse = (points[ring + 1].x - points[ring].x) < 0;

        for (int i = 0; i < countPerRing; i++) {
            // define point indices for quad
            unsigned int p1 = i + ring * countPerRing;
            unsigned int p2 = (i + 1) % countPerRing + ring * countPerRing;
            unsigned int p3 = p1 + countPerRing;
            unsigned int p4 = p2 + countPerRing;

            indices.insert(indices.end(), {
                    p1, p3, p2,
                    p2, p3, p4,
            });
        }
    }


    return {vertices, indices};
}
