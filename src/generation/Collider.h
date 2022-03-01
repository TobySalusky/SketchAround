//
// Created by Tobiathan on 2/17/22.
//

#ifndef SENIORRESEARCH_COLLIDER_H
#define SENIORRESEARCH_COLLIDER_H

#include "../util/Util.h"

struct NearestInfo {
    float partialArcLength;
    float totalArcLength;
    Vec2 minPoint;
    int index;
};

struct DistanceToLineSegmentInfo {
    float distance;
    Vec2 projection;
};

class Collider {
public:
    static NearestInfo NearestArcLengthToPoint(Vec2 p, const Vec2List& vec);

private:
    static DistanceToLineSegmentInfo DistanceToLineSegment(Vec2 p, Vec2 v1, Vec2 v2);
};


#endif //SENIORRESEARCH_COLLIDER_H
