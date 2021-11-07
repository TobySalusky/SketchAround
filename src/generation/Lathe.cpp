//
// Created by Tobiathan on 11/7/21.
//

#include "Lathe.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"
#include "Revolver.h"

void Lathe::HyperParameterUI() {

    const auto BindUIMeshUpdate = [&]() {
        if (ImGui::IsItemActive())
            UpdateMesh();
    };
    ImGui::SliderFloat("scale-radius", &scaleRadius, 0.1f, 3.0f);
    BindUIMeshUpdate();

    ImGui::SliderFloat("scale-z", &scaleZ, 0.1f, 3.0f);
    BindUIMeshUpdate();

    ImGui::SliderFloat("scale-y", &scaleY, 0.1f, 3.0f);
    BindUIMeshUpdate();

    ImGui::SliderFloat("lean-scalar", &leanScalar, 0.0f, 1.0f);
    BindUIMeshUpdate();

    ImGui::SliderInt("count-per-ring", &countPerRing, 3, 40);
    BindUIMeshUpdate();

    ImGui::SliderFloat("sample-length", &sampleLength, 0.01f, 0.5f);
    BindUIMeshUpdate();

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
}

void Lathe::AuxParameterUI() {
    if (ImGui::CollapsingHeader("Aux")) {
        ImGui::ColorEdit3("model-color", (float *) &color);
        ImGui::ColorEdit3("plot-color", (float *) &plotColor);
        ImGui::ColorEdit3("graph-y-color", (float *) &graphColorY);
        ImGui::ColorEdit3("graph-z-color", (float *) &graphColorZ);
        ImGui::SliderFloat3("translate", (float *) &modelTranslation, -5.f, 5.f);
    }
}

void Lathe::ModeSetUI(Enums::DrawMode drawMode) {
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

                glm::vec newCameraPos = renderInfo.camera.GetPos();
                newCameraPos.x = onScreen.x;
                renderInfo.camera.SetPos(newCameraPos);
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
