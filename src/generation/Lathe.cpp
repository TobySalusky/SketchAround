//
// Created by Tobiathan on 10/26/21.
//

#include "Lathe.h"
#include "Revolver.h"
#include "Sampler.h"
#include "CrossSectionTracer.h"

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
    BindUIMeshUpdate();

}

void Lathe::UpdateMesh() {
    /*if (!plottedPoints.empty()) {
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
    }*/
    if (plottedPoints.size() >=2 && graphedPointsY.size() >= 2) {
        const auto sampled = Sampler::DumbSample(plottedPoints, sampleLength);
        const auto sampled2 = Sampler::DumbSample(graphedPointsY, sampleLength);
        mesh.Set(CrossSectionTracer::Trace(sampled, sampled2, {}));
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
