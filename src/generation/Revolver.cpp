//
// Created by Tobiathan on 9/18/21.
//

#include <vector>
#include <cmath>
#include <tuple>


#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"

#include "Revolver.h"
#include "../graphing/Function.h"


// TODO: wrap end faces!!! (currently hollow)
std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Revolver::Revolve(const std::vector<glm::vec2>& points, const RevolveData& revolveData) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    const bool auxData = revolveData.auxPtr != nullptr;
    const int countPerRing = revolveData.countPerRing;

    for (const auto &point : points) {
        float translateY = !auxData ? 0 : Function::GetY(*revolveData.auxPtr, point.x);
        float angleLean = !auxData ? 0 : Function::GetSlopeRadians(*revolveData.auxPtr, point.x);
        for (int i = 0; i < countPerRing; i++) {
            float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) countPerRing));
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f,0.0f,0.0f)); // TODO: split mat creation from loop (b/c constant angles)
            glm::vec4 vec = rot * glm::vec4(0.0f, point.y * revolveData.scaleRadius, 0.0f, 1.0f);
            if (auxData) {
                printf("%f\n", angleLean);
                vec = glm::rotate(glm::mat4(1.0f), angleLean * revolveData.leanScalar, {0.0f, 0.0f, 1.0f}) * vec;
            }
            vec.x += point.x;
            vertices.emplace_back(glm::vec3(vec.x, vec.y * revolveData.scaleY + translateY, vec.z * revolveData.scaleZ));
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
                        p2, p3, p1,
                        p4, p3, p2,
                });
            } else {
                indices.insert(indices.end(), {
                        p1, p3, p2,
                        p2, p3, p4,
                });
            }
        }
    }

    return std::make_tuple(vertices, indices);
}
