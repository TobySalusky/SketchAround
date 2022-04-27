//
// Created by Tobiathan on 4/21/22.
//

#include "Bezier.h"

Vec2 Bezier::BezierPoint(float t, Vec2 c1, Vec2 p1, Vec2 p2, Vec2 c2) {
	// needs no explanation, pretty trivial stuff
	const float tt = t*t;
	const float ttt = tt * t;

	const float q1 = -ttt + 2.0f * tt - t;
	const float q2 = 3.0f * ttt - 5.0f * tt + 2.0f;
	const float q3 = -3.0f * ttt + 4.0f * tt + t;
	const float q4 = ttt - tt;

	return ((c1 * q1) + (p1 * q2) + (p2 * q3) + (c2 * q4)) * 0.5f;
}

Vec2List Bezier::BezierSplineCurve(const Vec2List &points) {

	if (points.size() < 2) {
		printf("[ERROR]: Bezier curves CAN NOT be constructed from less than 2 points\n");
		return {};
	}

	// first and last points have controls extended out linearly
	const auto WrappedGetPoint = [&](int i) {
		if (i == -1) {
			return 2.0f * points[0] - points[1];
		}
		if (i == points.size()) {
			return 2.0f * points[i - 1] - points[i - 2];
		}
		return points[i];
	};

	Vec2List outputPoints = {};
	const size_t pointsPerSegment = 10; // TODO: calc dynamically
	const size_t reserveSize = (points.size() - 1) * pointsPerSegment + 1;
	outputPoints.reserve(reserveSize);

	// runs once per line segment
	for (int i = 0; i < points.size() - 1; i++) {
		for (int j = 0; j < pointsPerSegment; j++) {
			const float t = (float) j * 0.1f;
			outputPoints.emplace_back(BezierPoint(t,
			 WrappedGetPoint(i - 1), WrappedGetPoint(i),
			 WrappedGetPoint(i + 1), WrappedGetPoint(i + 2)));
		}
	}
	outputPoints.emplace_back(points[points.size() - 1]);

	return outputPoints;
}
