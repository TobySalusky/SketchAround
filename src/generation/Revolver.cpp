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
#include "../util/Util.h"
#include "Sampler.h"
#include "LineAnalyzer.h"


std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Revolver::Revolve(const std::vector<glm::vec2>& points, const RevolveData& revolveData, TopologyCorrector* outTopologyData) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    const bool hasGraphY = !revolveData.graphY.empty();
    const bool hasGraphZ = !revolveData.graphZ.empty();
    const int countPerRing = revolveData.countPerRing;

    bool hasVariableCrossSection = revolveData.crossSectionSnapPointFunc.has_value();

    bool hasStaticCrossSection = revolveData.crossSectionPoints.size() >= 3;
    Vec2List sampledCrossSection;
    if (hasStaticCrossSection && !hasVariableCrossSection) {
        sampledCrossSection = Sampler::SampleTo(revolveData.crossSectionPoints, countPerRing);
    }

    bool hasCrossSection = hasVariableCrossSection || hasStaticCrossSection;

    bool requireArcLength = hasVariableCrossSection;
    float currentArcLength = 0.0f;
    float totalArcLength = requireArcLength ? LineAnalyzer::FullLength(points) : 0.0f;

    const auto GenVec = [&](int i, const Vec2& point){

        if (hasCrossSection) {
            Vec2 vec2D = sampledCrossSection[i] * point.y * revolveData.scaleRadius;
            return Vec4(0.0f, vec2D.y, vec2D.x, 1.0f);
        }
        float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) revolveData.countPerRing));
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f,0.0f,0.0f)); // TODO: split mat creation from loop (b/c constant angles)
        return rot * glm::vec4(0.0f, point.y * revolveData.scaleRadius, 0.0f, 1.0f);
    };

    for (int i = 0; i < points.size(); i++) {
        const auto& point = points[i];
        if (requireArcLength && i > 0) {
            currentArcLength += glm::length(point - points[i - 1]);
        }
        if (hasVariableCrossSection) { // TODO: optimize (cache?) incredibly intensive!!!
            sampledCrossSection = Sampler::SampleTo(revolveData.crossSectionSnapPointFunc.value()(currentArcLength / totalArcLength), countPerRing);
        }

        float translateY = !hasGraphY ? 0 : Function::GetY(revolveData.graphY, point.x);
        float angleLeanY = !hasGraphY ? 0 : Function::GetAverageRadians(revolveData.graphY, point.x, 5);

        float translateZ = !hasGraphZ ? 0 : Function::GetY(revolveData.graphZ, point.x);
        float angleLeanZ = !hasGraphZ ? 0 : -Function::GetAverageRadians(revolveData.graphZ, point.x, 5);

        for (int i = 0; i < countPerRing; i++) {
            Vec4 vec = GenVec(i, point);

            if (hasGraphY) {
                vec = glm::rotate(glm::mat4(1.0f), angleLeanY * revolveData.leanScalar, {0.0f, 0.0f, 1.0f}) * vec;
            }
            if (hasGraphZ) { // FIXME: this is so not right!!
                vec = glm::rotate(glm::mat4(1.0f), angleLeanZ * revolveData.leanScalar, {0.0f, 1.0f, 0.0f}) * vec;
            }
            vec.x += point.x;
            vertices.emplace_back(glm::vec3(vec.x, vec.y * revolveData.scaleY + translateY, vec.z * revolveData.scaleZ + translateZ));
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

    const int sideQuadCount = (int) (points.size() - 1) * countPerRing;

    const auto WrapEnd = [&](unsigned int startIndex, bool reverse = false) {
        for (int i = 0; i < countPerRing - 2; i++) {
            if (!reverse) {
                indices.insert(indices.end(), {
                        startIndex, (startIndex + 1 + i), (startIndex + 2 + i),
                });
            } else {
                indices.insert(indices.end(), {
                        (startIndex + 2 + i), (startIndex + 1 + i), startIndex,
                });
            }
        }
    };

    int wrapStartPoints = 0, wrapEndPoints = 0;

    if (revolveData.wrapStart) {
        WrapEnd(0);
        wrapStartPoints = countPerRing;
    }
    if (revolveData.wrapEnd) {
        WrapEnd((points.size() - 1) * countPerRing, true);
        wrapEndPoints = countPerRing;
    }

    if (outTopologyData != nullptr) *outTopologyData = {sideQuadCount, wrapStartPoints, wrapEndPoints};

    return {vertices, indices};
}
