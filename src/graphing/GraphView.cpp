//
// Created by Tobiathan on 10/19/21.
//

#include <glad.h>
#include <glfw3.h>
#include "GraphView.h"
#include "../vendor/glm/ext/matrix_transform.hpp"

glm::mat4 GraphView::GenProjection() const {

    const auto translateMat = glm::translate(glm::mat4(1.0f), Vec3(-centerAt, 0.0f));
    const auto scaleMat = glm::scale(glm::mat4(1.0f), Vec3(GenScaleVectorMat(), 1.0f));

    return scaleMat * translateMat;
}

void GraphView::Update(GraphViewUpdateInfo info) {
    const auto&[input, newGuiRect] = info;

    guiRect = newGuiRect;

    const Vec2 onCanvas = MousePosToCoords(input.GetMouse());

    if (input.mouseScroll != 0.0f && Util::VecIsNormalizedNP(Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect))) {
        Vec2 worldFrom = MousePosToCoords(input.GetMouse());
        scale += input.mouseScroll * 0.02f;
        scale = std::clamp(scale, 0.05f, 10.0f);
        Vec2 worldTo = MousePosToCoords(input.GetMouse());
        centerAt -= (worldTo - worldFrom);
    }

    if (input.mouseMiddlePressed && Util::VecIsNormalizedNP(Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect))) {
        enablePanOrDrag = true;
        panStart = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
    }

    if (input.mouseMiddleUnpressed) {
        enablePanOrDrag = false;
    }

    if (enablePanOrDrag && input.mouseMiddleDown) {
        if (input.Down(GLFW_KEY_LEFT_SHIFT)) {
            const auto lastPosNP = Util::NormalizeToRectNPFlipped(input.GetLastMouse(), guiRect) * GenAspectRatio();
            const auto posNP = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect) * GenAspectRatio();
            const float factor = glm::length(posNP) / glm::length(lastPosNP);
            scale *= factor;
        } else {
            const auto lastPos = MousePosToCoords(input.GetLastMouse());
            const auto pos = MousePosToCoords(input.GetMouse());

            centerAt += lastPos - pos;
        }
    }
}

Vec2 GraphView::MousePosToCoords(Vec2 mousePos) {
    return MousePosNPToCoords(Util::NormalizeToRectNPFlipped(mousePos, guiRect));
}

Vec2 GraphView::MousePosNPToCoords(Vec2 mousePosNP) {
    return (mousePosNP) * GenScaleVector() + centerAt;
}

Vec2 GraphView::GenScaleVector() const {
    return {1.0f / scale, (guiRect.height / guiRect.width) / scale};
}

Vec2 GraphView::GenScaleVectorMat() const {
    return {scale, scale * (guiRect.width / guiRect.height)};
}

Vec2 GraphView::GenAspectRatio() const {
    return {1.0f, guiRect.height / guiRect.width};
}

void GraphView::Render(GraphViewRenderInfo info) {
    const auto&[plot, input] = info;

    const Vec2 diff = GenScaleVector();
    Vec4 axisColor = {0.4f, 0.4f, 0.4f, 1.0f};
    Vec4 tickMarkColor = {0.7f, 0.7f, 0.7f, 1.0f};
    plot.AddLines({{centerAt.x - diff.x, 0.0f}, {centerAt.x + diff.x, 0.0f}}, axisColor, 0.003f);
    plot.AddLines({{0.0f, centerAt.y - diff.y}, {0.0f, centerAt.y + diff.y}}, axisColor, 0.003f);

    // TODO: tick marks

    const float tickMarkSideLength = 0.05f;
    if (scale > 0.01f) { // too small to be visible at certain point
        // on x-axis
        for (int i = (int)(centerAt.x + diff.x); (float) i > centerAt.x - diff.x; i--) {
            if (i == 0) continue;
            plot.AddLines({{(float) i, -tickMarkSideLength}, {(float) i, tickMarkSideLength}}, tickMarkColor, 0.003f);
        }

        // on y-axis
        for (int i = (int)(centerAt.y + diff.y); (float) i > centerAt.y - diff.y; i--) {
            if (i == 0) continue;
            plot.AddLines({{-tickMarkSideLength, (float) i}, {tickMarkSideLength, (float) i}}, tickMarkColor, 0.003f);
        }
    }

    // >> GIZMOS
    const Vec2 onCanvas = MousePosToCoords(input.GetMouse());

    // line gizmo
    {
        float col = 0.0f;
        plot.AddLines({{onCanvas.x, centerAt.y - diff.y},{onCanvas.x, centerAt.y + diff.y}}, {col, col, col, 0.15f}, 0.001f);
    }

    // mouse scroll gizmo
    if (enablePanOrDrag) {
        const RGBA panDragColor = {0.3f, 1.0f, 0.3f, 0.8f};
        if (input.Down(GLFW_KEY_LEFT_SHIFT)) {
            plot.AddLines({centerAt, onCanvas}, panDragColor, 0.0025f);
        } else {
            plot.AddLines({MousePosNPToCoords(panStart), onCanvas}, panDragColor, 0.0025f);
        }
    }
}

void GraphView::ReCenter() {
    scale = 1.0f;
    centerAt = {0.0f, 0.0f};
}
