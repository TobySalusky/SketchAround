//
// Created by Tobiathan on 11/2/21.
//

#ifndef SENIORRESEARCH_CROSSSECTIONTRACER_H
#define SENIORRESEARCH_CROSSSECTIONTRACER_H

#include <vector>
#include "../vendor/glm/vec3.hpp"
#include "../vendor/glm/vec2.hpp"
#include "../util/Includes.h"
#include "TopologyCorrector.h"

class CrossSectionTracer {
public:
    struct CrossSectionTraceData {
        int countPerRing{};
        bool wrapStart{};
        bool wrapEnd{};
        float sampleLength{};
        Vec2List& crossSectionPoints;
    };

    struct Segment {
        Vec2 p1{};
        Vec2 p2{};
    };

    static Vec2List AutoGenChordalAxis(const Vec2List& boundPoints, float sampleLength);

    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
    Trace(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data, TopologyCorrector* outTopologyData = nullptr);

    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
    Inflate(const std::vector<Segment> &segments, const CrossSectionTraceData& data, TopologyCorrector* outTopologyData = nullptr);

    static std::vector<Segment>
    TraceSegments(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data);
};


#endif //SENIORRESEARCH_CROSSSECTIONTRACER_H
