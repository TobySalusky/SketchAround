//
// Created by Tobiathan on 9/18/21.
//

#include <vector>
#include <cmath>
#include <tuple>


#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"

#include "Revolver.h"


// TODO: wrap end faces!!! (currently hollow)
std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Revolver::Revolve(const std::vector<glm::vec2>& points, int countPerRing) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    for (const auto &point : points) {
        for (int i = 0; i < countPerRing; i++) {
            float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) countPerRing));
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f,0.0f,0.0f)); // TODO: split mat creation from loop (b/c constant angles)
            glm::vec4 vec = rot * glm::vec4(point, 0.0f, 1.0f);
            vertices.emplace_back(glm::vec3(vec.x, vec.y, vec.z));
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

            if (reverse) {
                indices.insert(indices.end(), {
                        p3, p2, p1,
                        p3, p4, p2,
                });
            } else {
                indices.insert(indices.end(), {
                        p1, p2, p3,
                        p2, p4, p3,
                });
            }
        }
    }

    return std::make_tuple(vertices, indices);
}
