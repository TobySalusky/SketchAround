//
// Created by Tobiathan on 2/17/22.
//

#ifndef SENIORRESEARCH_COLLIDER_H
#define SENIORRESEARCH_COLLIDER_H

#include "../util/Util.h"

struct NearestInfo {

};

class Collider {
public:
    static NearestInfo NearestLineSegmentToPoint(Vec2 p, const Vec2List& vec);

private:
    static float DistanceToLineSegment(Vec2 p, Vec2 v1, Vec2 v2);
};


#endif //SENIORRESEARCH_COLLIDER_H
