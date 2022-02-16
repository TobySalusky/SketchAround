//
// Created by Tobiathan on 11/7/21.
//

#include "Lathe.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"
#include "Revolver.h"
#include "../animation/Timeline.h" // FIXME? cyclical dependency sus
#include "../util/Linq.h"
#include "LineAnalyzer.h"
#include "../graphing/Function.h"

void Lathe::HyperParameterUI(const UIInfo& info) {

    const auto BindUIMeshUpdate = [&] { // TODO: extract as ModelObject class funcs
        if (ImGui::IsItemActive())
            UpdateMesh();
    };

    const auto&[timeline] = info;

    AnimatableSliderValUpdateBound("scale-radius", &scaleRadius, timeline, 0.1f);

    AnimatableSliderValUpdateBound("scale-z", &scaleZ, timeline, 0.1f);

    AnimatableSliderValUpdateBound("scale-y", &scaleY, timeline, 0.1f);

    AnimatableSliderValUpdateBound("lean-scalar", &leanScalar, timeline, 0.0f, 1.0f);

    // TODO:
    ImGui::SliderInt("count-per-ring", &countPerRing, 3, 40);
    BindUIMeshUpdate();

    AnimatableSliderValUpdateBound("sample-length", &sampleLength, timeline, 0.01f, 0.5f);

    if (ImGui::Checkbox("wrap-start", &wrapStart)) {
        UpdateMesh();
    }

    ImGui::SameLine();
    if (ImGui::Checkbox("wrap-end", &wrapEnd)) {
        UpdateMesh();
    }
}

std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> Lathe::GenMeshTuple(TopologyCorrector* outTopologyData) {

    if (!plottedPoints.empty()) {
        const auto sampled = Sampler::DumbSample(plottedPoints, sampleLength);
        return Revolver::Revolve(sampled, {
                .scaleRadius=scaleRadius,
                .scaleZ=scaleZ,
                .scaleY=scaleY,
                .countPerRing=countPerRing,
                .leanScalar=leanScalar,
                .wrapStart=wrapStart,
                .wrapEnd=wrapEnd,
                .graphY=graphedPointsY,
                .graphZ=graphedPointsZ,
                .crossSectionPoints=crossSectionPoints,
        }, outTopologyData);
    }
    return {{}, {}};
}

void Lathe::UpdateMesh() {
    mesh.Set(GenMeshTuple());
}

void Lathe::RenderSelf2D(RenderInfo2D renderInfo) {
    renderInfo.plot.AddLines(graphedPointsY, graphColorY);
    renderInfo.plot.AddLines(graphedPointsZ, graphColorZ);
    renderInfo.plot.AddLines(plottedPoints, plotColor);
    renderInfo.plot.AddLines(crossSectionPoints, {1.0f, 0.0f, 1.0f, 1.0f});
}

void Lathe::RenderGizmos2D(RenderInfo2D renderInfo) {
    if (renderInfo.drawMode == Enums::MODE_GRAPH_Y) FunctionalAngleGizmo(renderInfo, graphedPointsY);
    else if (renderInfo.drawMode == Enums::MODE_GRAPH_Z) FunctionalAngleGizmo(renderInfo, graphedPointsZ);

//    if (plottedPoints.size() >= 2) { // tangent TEST gizmos
//        const float step = 0.01f;
//        const float arcLen = LineAnalyzer::FullLength(plottedPoints);
//        for (int i = 0; i < (int) (arcLen / step); i++) {
//            const Ray2D tangentRay = Function::GetRayAtLength(plottedPoints, (float) i * step);
//            const Vec2 p = tangentRay.origin;
//            const Vec2 perpendicular = Util::Perpendicular(tangentRay.dir);
//            renderInfo.plot.AddLines({p, p + glm::normalize(perpendicular) * 0.1f}, {0.7f, 0.3f, 0.7f, 1.0f}, 0.002f);
//        }
//    }

    const auto WrapGizmo = [&](glm::vec2 startPos) {
        // TODO: make dotted-line
        renderInfo.plot.AddLines({startPos, {startPos.x, 0.0f}}, {0.7f, 0.85f, 0.7f, 1.0f}, 0.002f);
    };
    if (plottedPoints.size() >= 2) {
        if (wrapStart) WrapGizmo(plottedPoints.front());
        if (wrapEnd) WrapGizmo(plottedPoints.back());
    }

    if (plottedPoints.size() >= 2) {
        renderInfo.plot.AddLines(Sampler::DumbSample(Linq::Select<Vec2, Vec2>(plottedPoints, [](Vec2 vec) {
            return vec * Vec2(1.0f, -1.0f);
        }), sampleLength), {0.0f, 0.0f, 0.0f, 0.2f}, 0.005f);
    }
}

void Lathe::ModeSetUI(Enums::DrawMode& drawMode) {
    ModeSet("Plot", Enums::DrawMode::MODE_PLOT, drawMode);
    ModeSet("Graph-Y", Enums::DrawMode::MODE_GRAPH_Y, drawMode);
    ModeSet("Graph-Z", Enums::DrawMode::MODE_GRAPH_Z, drawMode);
    ModeSet("Cross-Section", Enums::DrawMode::MODE_CROSS_SECTION, drawMode);
}

void Lathe::ClearAll() {
    graphedPointsY.clear();
    graphedPointsZ.clear();
    plottedPoints.clear();
    crossSectionPoints.clear();
    DiffPoints(Enums::MODE_PLOT);
    DiffPoints(Enums::MODE_GRAPH_Y);
    DiffPoints(Enums::MODE_GRAPH_Z);
    DiffPoints(Enums::MODE_CROSS_SECTION);
}

std::vector<glm::vec2>& Lathe::GetPointsRefByMode(Enums::DrawMode drawMode) {
    if (drawMode == Enums::MODE_GRAPH_Y) return graphedPointsY;
    if (drawMode == Enums::MODE_GRAPH_Z) return graphedPointsZ;
    if (drawMode == Enums::MODE_CROSS_SECTION) return crossSectionPoints;
    return plottedPoints;
}

Enums::LineType Lathe::LineTypeByMode(Enums::DrawMode drawMode) {
    if (drawMode == Enums::MODE_PLOT || drawMode == Enums::MODE_CROSS_SECTION) return Enums::POLYLINE;
    return Enums::PIECEWISE;
}

ModelObject *Lathe::CopyInternals() { // FIXME: sus
#define QUICK_COPY(a) copy->a = a
    auto* copy = new Lathe();
    QUICK_COPY(wrapStart);
    QUICK_COPY(wrapEnd);
    QUICK_COPY(plottedPoints);
    QUICK_COPY(graphedPointsY);
    QUICK_COPY(graphedPointsZ);
    QUICK_COPY(crossSectionPoints);
    QUICK_COPY(countPerRing);
    QUICK_COPY(scaleRadius);
    QUICK_COPY(scaleY);
    QUICK_COPY(scaleZ);
    QUICK_COPY(leanScalar);
    return copy;
}
