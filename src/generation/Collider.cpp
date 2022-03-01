//
// Created by Tobiathan on 2/17/22.
//

#include "Collider.h"

DistanceToLineSegmentInfo Collider::DistanceToLineSegment(Vec2 p, Vec2 v1, Vec2 v2) {
    const float l2 = pow(glm::length(v2 - v1), 2.0f); // avoids sqrt
    if (l2 == 0.0) return {glm::length(v1 - p), v1};
    const float t = fmax(0.0f, fmin(1.0f, glm::dot(p - v1, v2 - v1) / l2));
    const Vec2 projection = v1 + t * (v2 - v1);  // Projection falls on the segment
    return {glm::length(projection - p), projection};
}

NearestInfo Collider::NearestArcLengthToPoint(Vec2 p, const Vec2List& vec) {

    float arcLength = 0.0f;

    float arcLengthMin = 0.0f;
    float minDistance = MAXFLOAT;
    Vec2 minPoint;
    int minIndex = -1;

    for (int i = 0; i < vec.size() - 1; i++) {
        const auto& [distance, projection] = DistanceToLineSegment(p, vec[i], vec[i + 1]);

        if (distance < minDistance) {
            minDistance = distance;
            arcLengthMin = arcLength + glm::length(projection - vec[i]);
            minPoint = projection;
            minIndex = i;
        }

        arcLength += glm::length(vec[i + 1] - vec[i]);
    }

    return {arcLengthMin, arcLength, minPoint, minIndex};
}
