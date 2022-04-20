//
// Created by Tobiathan on 11/12/21.
//

#include "EditingContext.h"

Vec2 EditingContext::ApplySnapGridIfAny(Vec2 actualCanvasPos) {
	switch (snapGrid) {
		case SnapGrid::Circle: {
			const float initMag = glm::length(drawingBeginCoords - snapGridCentralPoint);
			const float currAngle = Util::Angle(actualCanvasPos - snapGridCentralPoint);

			return snapGridCentralPoint + Util::Polar(initMag, -currAngle + (float) M_PI_2);
		}
		case SnapGrid::None:
		default:
			return actualCanvasPos;
	}
}

void EditingContext::RenderGizmos(Mesh2D &plot, GraphView &graphView) {
	const RGBA orange = {1.0f, 0.7f, 0.0f, 0.6f};
	const RGBA blue = {0.2f, 0.1f, 1.0f, 0.6f};
	const RGBA red = {1.0f, 0.1f, 0.3f, 0.6f};

	switch (snapGrid) { // render snapping grids
		case SnapGrid::Circle: {
			constexpr RGBA circleGridColour {0.0f, 0.0f, 0.0f, 0.1f};
			constexpr RGBA circleGridColourActive {1.0f, 0.0f, 0.0f, 0.3f};

			const auto CircRad = [&](float radius, const RGBA& colour) {
				plot.AddPolygonOutline(snapGridCentralPoint, radius, 60, colour, 0.005f);
			};

			plot.AddPolygonOutline(snapGridCentralPoint,
			                       0.01f, 20, circleGridColourActive, 0.01f);

			for (size_t i = 0; i < 20; i++) { // TODO: calc num of circles to properly fill screen (min thru max)
				CircRad(0.125f * (float) (i + 1), circleGridColour);
			}

			if (isDrawing) {
				CircRad(graphView.scale * glm::length(drawingBeginCoords - snapGridCentralPoint), circleGridColourActive);
			}
		}
		default:
			break;
	}

	if (transformationActive) {
		switch (transformationType) {
			case Enums::TRANSFORM_DRAG: {
				plot.AddLines({primaryGizmoPoint, secondaryGizmoPoint}, orange, 0.005f);
				break;
			}
			case Enums::TRANSFORM_ROTATE: {
				plot.AddLines({primaryGizmoPoint, secondaryGizmoPoint}, orange, 0.005f);

				Vec2List rotateGizmo;
				float initAngle = Util::Angle(GetTransformStartPos() - primaryGizmoPoint);
				float newAngle = Util::Angle(secondaryGizmoPoint - primaryGizmoPoint);
				float angleDiff = newAngle - initAngle;

				const float angleStep = 0.1f, gizmoRad = 0.2f * plot.GenScaleLineMult();

				int pCount = (int) (std::fabs(angleDiff) / angleStep);
				rotateGizmo.reserve(pCount + 2);
				rotateGizmo.emplace_back(primaryGizmoPoint);
				rotateGizmo.emplace_back(primaryGizmoPoint + glm::normalize(GetTransformStartPos() - primaryGizmoPoint) * gizmoRad);
				for (int i = 0; i < pCount; i++) {
					rotateGizmo.emplace_back(primaryGizmoPoint + Util::Polar(gizmoRad, (-initAngle + (float) M_PI_2) + (float) i * angleStep * (float) -Util::Signum(angleDiff)));
				}

				plot.AddLines(rotateGizmo, blue, 0.005f);
				break;
			}
			case Enums::TRANSFORM_SCALE: {
				plot.AddLines({primaryGizmoPoint, secondaryGizmoPoint}, orange, 0.005f);
				const Vec2 dir = glm::normalize(secondaryGizmoPoint - primaryGizmoPoint);
				const auto PointAlongAt = [&](Vec2 magTowards) {
					return primaryGizmoPoint + (glm::length(magTowards - primaryGizmoPoint) * dir);
				};
				const RGBA diffColor = glm::length(GetTransformStartPos() - primaryGizmoPoint) <
				                       glm::length(secondaryGizmoPoint - primaryGizmoPoint) ? blue : red;
				plot.AddLines({PointAlongAt(GetTransformStartPos()), PointAlongAt(secondaryGizmoPoint)}, diffColor,
				              0.005f);
				break;
			}
			case Enums::TRANSFORM_SMEAR: {
				break;
			}
		}
	}
}
