//
// Created by Tobiathan on 10/26/21.
//

#include "ModelObject.h"
#include "Revolver.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"
#include "../graphing/Function.h"
#include "../util/Util.h"

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
