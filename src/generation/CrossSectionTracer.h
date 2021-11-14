//
// Created by Tobiathan on 11/2/21.
//

#ifndef SENIORRESEARCH_CROSSSECTIONTRACER_H
#define SENIORRESEARCH_CROSSSECTIONTRACER_H

#include <vector>
#include "../vendor/glm/vec3.hpp"
#include "../vendor/glm/vec2.hpp"

class CrossSectionTracer {
public:
    struct CrossSectionTraceData {
        int countPerRing{};
        bool wrapStart{};
        bool wrapEnd{};
    };

    struct Segment {
        glm::vec2 p1{}, p2{};
    };

    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Trace(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data);
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Inflate(const std::vector<Segment> &segments, const CrossSectionTraceData& data);
    static std::vector<Segment> TraceSegments(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, const CrossSectionTraceData& data);
private:
};


#endif //SENIORRESEARCH_CROSSSECTIONTRACER_H
