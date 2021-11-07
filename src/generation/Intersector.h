//
// Created by Tobiathan on 11/2/21.
//

#ifndef SENIORRESEARCH_INTERSECTOR_H
#define SENIORRESEARCH_INTERSECTOR_H


#include "../vendor/glm/vec2.hpp"

#include <optional>

class Intersector {
public:
    static bool Segment(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2);
    static std::optional<glm::vec2> RaySegment(glm::vec2 rayOrigin, glm::vec2 rayDirVec, glm::vec2 p1, glm::vec2 p2);
private:
    static int SegmentOrientation(glm::vec2 p, glm::vec2 q, glm::vec2 r);
};


#endif //SENIORRESEARCH_INTERSECTOR_H
