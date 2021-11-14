//
// Created by Tobiathan on 11/7/21.
//

#include "Lathe.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"
#include "Revolver.h"
#include "../animation/Timeline.h" // FIXME? cyclical dependency sus

void Lathe::HyperParameterUI(const UIInfo& info) {

    const auto BindUIMeshUpdate = [&] { // TODO: extract as ModelObject class funcs
        if (ImGui::IsItemActive())
            UpdateMesh();
    };

    const auto AnimatableSliderValUpdateBound = [&] (const std::string& label, float* ptr, float min, float max) {
        bool animated = info.timeline.HasFloatLayer(label);
        ImGui::Checkbox(("##" + label).c_str(), &animated);
        if (ImGui::IsItemClicked()) {
            if (!animated) {
                info.timeline.AddFloatLayer(label, ptr);
            } else {
                info.timeline.RemoveFloatLayer(label);
            }
        }

        ImGui::SameLine();
        ImGui::SliderFloat(label.c_str(), ptr, min, max);
        if (ImGui::IsItemActive()) {
            if (animated) {
                info.timeline.UpdateFloat(label, *ptr);
            }
            UpdateMesh();
        }
    };

    AnimatableSliderValUpdateBound("scale-radius", &scaleRadius, 0.1f, 3.0f);

    AnimatableSliderValUpdateBound("scale-z", &scaleZ, 0.1f, 3.0f);

    AnimatableSliderValUpdateBound("scale-y", &scaleY, 0.1f, 3.0f);

    AnimatableSliderValUpdateBound("lean-scalar", &leanScalar, 0.0f, 1.0f);

    // TODO:
    ImGui::SliderInt("count-per-ring", &countPerRing, 3, 40);
    BindUIMeshUpdate();

    AnimatableSliderValUpdateBound("sample-length", &sampleLength, 0.01f, 0.5f);

    if (ImGui::Checkbox("wrap-start", &wrapStart)) {
        UpdateMesh();
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("wrap-end", &wrapEnd)) {
        UpdateMesh();
    }
}

void Lathe::UpdateMesh() {
    if (!plottedPoints.empty()) {
        const auto sampled = Sampler::DumbSample(plottedPoints, sampleLength);
        mesh.Set(Revolver::Revolve(sampled, {
                .scaleRadius=scaleRadius,
                .scaleZ=scaleZ,
                .scaleY=scaleY,
                .countPerRing=countPerRing,
                .leanScalar=leanScalar,
                .wrapStart=wrapStart,
                .wrapEnd=wrapEnd,
                .graphYPtr=&(graphedPointsY),
                .graphZPtr=&(graphedPointsZ),
        }));
    } else {
        GLfloat vertices[] = {
                -1.0f, -1.0f, 0.0f,
                0.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
        };

        GLuint indices[] = {
                0, 3, 1,
                1, 3, 2,
                2, 3, 0,
                0, 1, 2
        };
        mesh.Set(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));
    };
}

void Lathe::RenderSelf2D(RenderInfo2D renderInfo) {
    renderInfo.plot.AddLines(graphedPointsY, graphColorY);
    renderInfo.plot.AddLines(graphedPointsZ, graphColorZ);
    renderInfo.plot.AddLines(plottedPoints, plotColor);
}

void Lathe::RenderGizmos2D(RenderInfo2D renderInfo) {
    if (renderInfo.drawMode == Enums::MODE_GRAPH_Y) FunctionalAngleGizmo(renderInfo, graphedPointsY);
    else if (renderInfo.drawMode == Enums::MODE_GRAPH_Z) FunctionalAngleGizmo(renderInfo, graphedPointsZ);

    const auto WrapGizmo = [&](glm::vec2 startPos) {
        // TODO: make dotted-line
        renderInfo.plot.AddLines({startPos, {startPos.x, 0.0f}}, {0.7f, 0.85f, 0.7f, 1.0f}, 0.002f);
    };
    if (plottedPoints.size() >= 2) {
        if (wrapStart) WrapGizmo(plottedPoints.front());
        if (wrapEnd) WrapGizmo(plottedPoints.back());
    }
}

void Lathe::AuxParameterUI(const UIInfo& info) {
    if (ImGui::CollapsingHeader("Aux")) {
        ImGui::ColorEdit3("model-color", (float *) &color);
        ImGui::ColorEdit3("plot-color", (float *) &plotColor);
        ImGui::ColorEdit3("graph-y-color", (float *) &graphColorY);
        ImGui::ColorEdit3("graph-z-color", (float *) &graphColorZ);
        ImGui::SliderFloat3("translate", (float *) &modelTranslation, -5.f, 5.f);
    }
}

void Lathe::ModeSetUI(Enums::DrawMode& drawMode) {
    ModeSet("Plot", Enums::DrawMode::MODE_PLOT, plottedPoints, drawMode);
    ModeSet("Graph-Y", Enums::DrawMode::MODE_GRAPH_Y, graphedPointsY, drawMode);
    ModeSet("Graph-Z", Enums::DrawMode::MODE_GRAPH_Z, graphedPointsZ, drawMode);
}

void Lathe::ClearAll() {
    graphedPointsY.clear();
    graphedPointsZ.clear();
    plottedPoints.clear();
}

void Lathe::ClearSingle(Enums::DrawMode drawMode) {
    if (drawMode == Enums::DrawMode::MODE_GRAPH_Y) graphedPointsY.clear();
    else if (drawMode == Enums::DrawMode::MODE_GRAPH_Z) graphedPointsZ.clear();
    else if (drawMode == Enums::DrawMode::MODE_PLOT) plottedPoints.clear();
}

void Lathe::InputPoints(MouseInputInfo renderInfo) {

    const auto& onScreen = renderInfo.onScreen;
    switch (renderInfo.drawMode) {
        case Enums::DrawMode::MODE_PLOT:
            if (plottedPoints.empty() || plottedPoints[plottedPoints.size() - 1] != onScreen) {
                plottedPoints.emplace_back(onScreen);
                //printf("%f\n", onScreen.x);
                UpdateMesh();


//                glm::vec newCameraPos = renderInfo.camera.GetPos();
//                newCameraPos.x = onScreen.x;
//                renderInfo.camera.SetPos(newCameraPos);
            }
            break;
        case Enums::DrawMode::MODE_GRAPH_Z:
        case Enums::DrawMode::MODE_GRAPH_Y:
            std::vector<glm::vec2>& vec = renderInfo.drawMode == Enums::DrawMode::MODE_GRAPH_Y ? graphedPointsY : graphedPointsZ;
            if (vec.empty() || onScreen.x > vec.back().x) {
                vec.emplace_back(onScreen);
                UpdateMesh();
            }
            break;
    }

}

std::vector<glm::vec2>& Lathe::GetPointsRefByMode(Enums::DrawMode drawMode) {
    if (drawMode == Enums::MODE_GRAPH_Y) return graphedPointsY;
    if (drawMode == Enums::MODE_GRAPH_Z) return graphedPointsZ;
    return plottedPoints;
}
