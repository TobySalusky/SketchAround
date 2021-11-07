//
// Created by Tobiathan on 11/2/21.
//

#ifndef SENIORRESEARCH_CROSSSECTIONTRACER_H
#define SENIORRESEARCH_CROSSSECTIONTRACER_H

#include <vector>
#include "../vendor/glm/vec3.hpp"
#include "../vendor/glm/vec2.hpp"


struct Segment {
    glm::vec2 p1{}, p2{};
};

struct CrossSectionTraceData {

};

class CrossSectionTracer {
public:
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Trace(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, CrossSectionTraceData data);
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Inflate(const std::vector<Segment> &segments, CrossSectionTraceData data);
    static std::vector<Segment> TraceSegments(const std::vector<glm::vec2> &points, const std::vector<glm::vec2> &pathTrace, CrossSectionTraceData data);
private:
};


#endif //SENIORRESEARCH_CROSSSECTIONTRACER_H
