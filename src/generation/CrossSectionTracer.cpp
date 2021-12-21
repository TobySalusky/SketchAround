//
// Created by Tobiathan on 11/2/21.
//

#include "CrossSectionTracer.h"
#include "Intersector.h"
#include "../vendor/glm/geometric.hpp"
#include "../vendor/glm/fwd.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"
#include "../util/Util.h"
#include "../graphing/Function.h"
#include "LineAnalyzer.h"
#include "../animation/LineLerper.h"

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
CrossSectionTracer::Trace(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data) {
    return Inflate(TraceSegments(points, pathTrace, data), data);
}

std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
CrossSectionTracer::Inflate(const std::vector<Segment> &segments, const CrossSectionTraceData& data) {
    const int countPerRing = data.countPerRing;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    for (const auto &segment : segments) {
        const auto midpoint = (segment.p1 + segment.p2) / 2.0f;
        const float radius = glm::length(midpoint - segment.p1);

        for (int i = 0; i < countPerRing; i++) {
            float angle = (float) M_PI * 2 * ((float) ((float) i * 1 / (float) countPerRing));
            glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f,0.0f,0.0f)); // TODO: split mat creation from loop (b/c constant angles)
            glm::vec4 vec = rot * glm::vec4(0, radius, 0.0f, 1.0f);

            const float leanAngle = Util::Angle(segment.p2 - segment.p1) + (float) M_PI_2;

            vec = glm::rotate(glm::mat4(1.0f), leanAngle, {0.0f, 0.0f, 1.0f}) * vec;

            vec.x += midpoint.x;

            vertices.emplace_back(glm::vec3(vec.x, vec.y + midpoint.y, vec.z));
        }
    }

    for (int ring = 0; ring < (int) segments.size() - 1; ring++) {

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

    const auto WrapEnd = [&](unsigned int startIndex) {
        for (int i = 0; i < countPerRing - 2; i++) {
            indices.insert(indices.end(), {
                    startIndex, (startIndex + 1 + i), (startIndex + 2 + i),
            });
        }
    };

    if (!segments.empty()) {
        if (data.wrapStart) WrapEnd(0);
        if (data.wrapEnd) WrapEnd((segments.size() - 1) * countPerRing);
    }

    return {vertices, indices};
}

std::vector<CrossSectionTracer::Segment>
CrossSectionTracer::TraceSegments(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data) {
    std::vector<Segment> segments;

    // TODO: optimize!!!
    for (const auto &pathPoint : pathTrace) {
        bool hasP1 = false, hasP2 = false;
        glm::vec2 p1, p2;
        for (int i = 0; i < points.size() - 1; i++) {

            const float angle = -Function::GetAverageRadians(pathTrace, pathPoint.x, 3);
            const auto dirVec = Util::Polar(angle);

            const auto intersectionPos = Intersector::RaySegment(pathPoint, dirVec, points[i], points[i + 1]);
            const auto intersectionNeg = Intersector::RaySegment(pathPoint, -dirVec, points[i], points[i + 1]);

            if (intersectionPos.has_value()) {
                if (!hasP1) {
                    p1 = intersectionPos.value();
                    hasP1 = true;
                } else {
                    if (glm::length(intersectionPos.value() - pathPoint) < glm::length(p1 - pathPoint)) {
                        p1 = intersectionPos.value();
                    }
                }
            }

            if (intersectionNeg.has_value()) {
                if (!hasP2) {
                    p2 = intersectionNeg.value();
                    hasP2 = true;
                } else {
                    if (glm::length(intersectionNeg.value() - pathPoint) < glm::length(p2 - pathPoint)) {
                        p2 = intersectionNeg.value();
                    }
                }
            }
        }

        if (hasP1 && hasP2) {
            // may not intersect with previous segment
            if (segments.empty() || !Intersector::Segment(p1, p2, segments.back().p1, segments.back().p2)) {
                segments.push_back({p1, p2});
            }
        }
    }

    return segments;
}

Vec2List CrossSectionTracer::AutoGenChordalAxis(const Vec2List &boundPoints, const float sampleLength) {
    const float halfway = LineAnalyzer::FullLength(boundPoints) / 2.0f;

    int midPointIndex = -1;
    float sumLength = 0.0f;
    for (int i = 0; i < boundPoints.size() - 1; i++) {
        sumLength += glm::length(boundPoints[i + 1] - boundPoints[i]);
        if (sumLength >= halfway) {
            midPointIndex = i + 1;
            break;
        }
    }

    if (midPointIndex == -1 || midPointIndex == boundPoints.size() - 1) return {};

    Vec2List secondHalf = {};
    secondHalf.reserve(boundPoints.size() - midPointIndex - 1);
    for (size_t i = boundPoints.size() - 1; i > midPointIndex; i--) {
        secondHalf.emplace_back(boundPoints[i]);
    }

    return LineLerper::MorphPolyLine(std::vector(boundPoints.begin(), boundPoints.begin() + midPointIndex),
                                     secondHalf,
                                     0.5f, ceil(halfway * 2.0f / sampleLength));
}
