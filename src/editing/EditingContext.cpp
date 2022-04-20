//
// Created by Tobiathan on 11/12/21.
//

#include "EditingContext.h"

Vec2 EditingContext::ApplySnapGridIfAny(Vec2 actualCanvasPos) {
	switch (snapGrid) {
		case SnapGrid::Circle: {
			const float initMag = glm::length(drawingBeginCoords - snapGridCentralPoint);
			const float currAngle = Util::Angle(actualCanvasPos - snapGridCentralPoint);

			return Util::Polar(initMag, -currAngle + (float) M_PI_2);
		}
		case SnapGrid::None:
		default:
			return actualCanvasPos;
	}
}
