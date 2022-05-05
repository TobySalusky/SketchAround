//
// Created by Tobiathan on 11/19/21.
//

#include "Display3DContext.h"
#include "../vendor/glm/ext/matrix_transform.hpp"

const float sensitivity = 4.0f;
const float scrollSensitivity = 0.1f;

void Display3DContext::Update(Display3DContextUpdateInfo info) {
    const auto& [input, camera, guiRect, focused] = info;

    Vec2 mousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);

    if (Util::VecIsNormalizedNP(mousePos)) {
        if (input.mouseDown && focused) {
            Vec2 diff = mousePos - Util::NormalizeToRectNPFlipped(input.GetLastMouse(), guiRect);

            const float angleDiffX = diff.x * sensitivity;
            const float angleDiffY = diff.y * sensitivity * (guiRect.height / guiRect.width);
            
            const float newAngleY = Camera::ClampedPhi(camera.phi + angleDiffY);

            camera.theta += angleDiffX;
            camera.phi = newAngleY;
			camera.CalcPosDir();
        }

        if (input.mouseScroll != 0.0f) {
            const float minMag = 0.3f;
            float newMag = fmax(camera.rho - input.mouseScroll * scrollSensitivity, minMag);
            camera.rho = newMag;
            camera.CalcPosDir();
        }
    }
}