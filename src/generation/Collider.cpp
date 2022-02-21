//
// Created by Tobiathan on 2/17/22.
//

#include "Collider.h"

float Collider::DistanceToLineSegment(Vec2 p, Vec2 v1, Vec2 v2) {
    const float l2 = pow(glm::length(v2 - v1), 2.0f); // avoids sqrt
    if (l2 == 0.0) return glm::length(v1 - p);
    const float t = fmax(0.0f, fmin(1.0f, glm::dot(p - v1, v2 - v1) / l2));
    const Vec2 projection = v1 + t * (v2 - v1);  // Projection falls on the segment
    return glm::length(projection - p);
}

NearestInfo Collider::NearestLineSegmentToPoint(Vec2 p, const Vec2List& vec) {
    return NearestInfo();
}
