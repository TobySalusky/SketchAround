//
// Created by Tobiathan on 10/26/21.
//

#include "ModelObject.h"
#include "Revolver.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"
#include "../graphing/Function.h"
#include "../util/Util.h"

#include <glfw3.h>

void ModelObject::Render3D(RenderInfo3D renderInfo) {
    renderInfo.shader3D.SetModel(GenModelMat());
    renderInfo.mainLight.SetColor(color);
    renderInfo.mainLight.Apply(renderInfo.shader3D);

    mesh.Render();
}

void ModelObject::Render2D(RenderInfo2D renderInfo) {
    RenderSelf2D(renderInfo);
    RenderGizmos2D(renderInfo);
}

void ModelObject::FunctionalAngleGizmo(RenderInfo2D renderInfo, const std::vector<glm::vec2>& points) {
    float pointY = Function::GetY(points, renderInfo.onScreen.x);
    float pointAngle = Util::SlopeToRadians(Function::GetAverageSlope(points, renderInfo.onScreen.x, 5));
    if (pointY != 0) {
        std::vector<glm::vec2> gizmo {{renderInfo.onScreen.x, pointY}, glm::vec2(renderInfo.onScreen.x, pointY) + Util::Polar(0.1f, -pointAngle)};
        renderInfo.plot.AddLines(gizmo, {1.0f, 0.0f, 1.0f, 1.0f});
    }
}

void ModelObject::RenderGizmos3D(RenderInfo3D renderInfo) {}

EditingContext::TransformStartInfo GenTransformStartInfo(EditingInfo info) {
    return {info.onScreen};
}

void ModelObject::EditCurrentLines(EditingInfo info) {
    auto& points = GetPointsRefByMode(info.drawMode);

    auto& editContext = info.editContext;
    auto& input = info.input;

    const auto Diff = [&](const std::function <glm::vec2(glm::vec2)>& vectorFunc) {
        for (auto& point : points) {
            point = vectorFunc(point);
        }
        UpdateMesh();
    };

    const auto DiffBase = [&](const std::function <glm::vec2(glm::vec2)>& vectorFunc) {
        for (int i = 0; i < points.size(); i++) {
            points[i] = vectorFunc(editContext.transformStoreInitPoints[i]);
        }
        UpdateMesh();
    };

    if (input.Pressed(GLFW_KEY_G)) editContext.StartTransform(Enums::TRANSFORM_DRAG, GenTransformStartInfo(info), points);
    if (input.Pressed(GLFW_KEY_R)) editContext.StartTransform(Enums::TRANSFORM_ROTATE, GenTransformStartInfo(info), points);
    if (input.Pressed(GLFW_KEY_S)) editContext.StartTransform(Enums::TRANSFORM_SCALE, GenTransformStartInfo(info), points);
    if (input.Pressed(GLFW_KEY_U)) editContext.StartTransform(Enums::TRANSFORM_SMEAR, GenTransformStartInfo(info), points);

    if (editContext.IsTransformationActive()) {
        switch (editContext.GetTransformationType()) {
            case Enums::TRANSFORM_DRAG: {
                glm::vec2 delta = info.onScreen - editContext.GetLastMousePos();
                Diff([=](glm::vec2 vec) {
                    return vec + delta;
                });
                break;
            }
            case Enums::TRANSFORM_ROTATE: {
                float lastAngle = Util::Angle(editContext.GetLastMousePos());
                float newAngle = Util::Angle(info.onScreen);
                float angleDiff = newAngle - lastAngle;
                Diff([=](glm::vec2 vec) {
                    float angle = Util::Angle(vec);
                    float mag = glm::length(vec);
                    return Util::Polar(mag, -(angle - (float) M_PI_2 + angleDiff));
                });
                break;
            }
            case Enums::TRANSFORM_SCALE: {
                float lastMag = glm::length(editContext.GetLastMousePos());
                float newMag = glm::length(info.onScreen);
                Diff([=](glm::vec2 vec) {
                    float angle = Util::Angle(vec);
                    float mag = glm::length(vec);
                    return Util::Polar(mag / lastMag * newMag, -(angle - (float) M_PI_2));
                });
                break;
            }
            case Enums::TRANSFORM_SMEAR: {
                const auto SmearFalloff = [](float x) {
                    return 1.0f / (1.0f + exp((x - 0.24f) * 20.0f));
                };

                glm::vec2 initPos = editContext.GetTransformStartPos();
                glm::vec2 delta = info.onScreen - initPos;
//                DiffBase([=](glm::vec2 vec) { // CIRCULAR DROP-OFF
//                    float initMag = glm::length(vec - initPos);
//                    float mult = sqrt(rad - initMag * initMag);
//                    if (std::isnan(mult)) mult = 0;
//                    return vec + delta * mult;
//                });
                DiffBase([=](glm::vec2 vec) {
                    float initMag = glm::length(vec - initPos);
                    return vec + delta * SmearFalloff(initMag);
                });
                break;
            }
        }

        if (input.mousePressed) {
            editContext.FinalizeTransform();
            editContext.DisableDrawingForClick();
        } else if (input.Pressed(GLFW_KEY_ENTER)) {
            editContext.FinalizeTransform();
        } else if (input.Pressed(GLFW_KEY_SPACE)) {
            editContext.CancelTransform();
        }

    } else {
        if (input.mouseDown && editContext.IsDrawingEnabledForClick()) {
            InputPoints({info.drawMode, info.onScreen, info.camera});
        }
    }


    // END ACTIONS
    if (input.mouseUnpressed) {
        editContext.OnMouseUp();
    }

    editContext.SetLastMousePos(info.onScreen);
}

void ModelObject::EditMakeup(EditingInfo info) {
    EditCurrentLines(info);
}
