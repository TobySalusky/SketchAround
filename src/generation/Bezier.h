//
// Created by Tobiathan on 4/21/22.
//

#ifndef SENIORRESEARCH_BEZIER_H
#define SENIORRESEARCH_BEZIER_H

#include "../util/Util.h"

class Bezier {
public:
	static Vec2 BezierPoint(float t, Vec2 c1, Vec2 p1, Vec2 p2, Vec2 c2);
	static Vec2List BezierSplineCurve(const Vec2List& points);
};


#endif //SENIORRESEARCH_BEZIER_H
