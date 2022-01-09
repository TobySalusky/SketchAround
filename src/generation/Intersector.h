//
// Created by Tobiathan on 11/2/21.
//

#ifndef SENIORRESEARCH_INTERSECTOR_H
#define SENIORRESEARCH_INTERSECTOR_H


#include "../vendor/glm/vec2.hpp"
#include "../util/Util.h"

#include <optional>

struct MeshIntersection {
    Vec3 pos;
    Vec3 normal;
};

struct Ray {
    Vec3 origin;
    Vec3 dir;
};

class Intersector {
public:
    static bool Segment(glm::vec2 p1, glm::vec2 q1, glm::vec2 p2, glm::vec2 q2);

    static std::optional<glm::vec2> RaySegment(glm::vec2 rayOrigin, glm::vec2 rayDirVec, glm::vec2 p1, glm::vec2 p2);

    static bool RayTriangleIntersection(Ray ray, Vec3 v0, Vec3 v1, Vec3 v2,
                                               MeshIntersection& outIntersection);

private:
    static int SegmentOrientation(glm::vec2 p, glm::vec2 q, glm::vec2 r);
};


#endif //SENIORRESEARCH_INTERSECTOR_H
