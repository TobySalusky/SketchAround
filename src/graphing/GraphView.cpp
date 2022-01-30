//
// Created by Tobiathan on 10/19/21.
//

#include <glew.h>
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

    if (input.mouseMiddleDown) {
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
