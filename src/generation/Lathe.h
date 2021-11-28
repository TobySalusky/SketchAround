//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_LATHE_H
#define SENIORRESEARCH_LATHE_H


#include "ModelObject.h"

class Lathe : public ModelObject {
public:
    void HyperParameterUI(const UIInfo& info) final;
    void AuxParameterUI(const UIInfo& info) final;
    void UpdateMesh() final;
    void ModeSetUI(Enums::DrawMode& drawMode) final;

    void ClearAll() override;


    void RenderSelf2D(RenderInfo2D renderInfo) final;
    void RenderGizmos2D(RenderInfo2D renderInfo) final;

    Vec2List& GetPointsRefByMode(Enums::DrawMode drawMode) final;

    Enums::LineType LineTypeByMode(Enums::DrawMode drawMode) final;

    std::tuple<Vec3List, std::vector<GLuint>> GenMeshTuple() final;

private:
    float scaleRadius = 1.0f, scaleZ = 1.0f, scaleY = 1.0f, leanScalar = 0.25f;
    int countPerRing = 10;

    bool wrapStart = false, wrapEnd = false;

    Vec2List plottedPoints;
    Vec2List graphedPointsY;
    Vec2List graphedPointsZ;
    Vec2List crossSectionPoints;

    RGBA plotColor = {0.0f, 0.0f, 0.0f, 1.0f};
    RGBA graphColorY = {0.0f, 0.0f, 1.0f, 1.0f};
    RGBA graphColorZ = {0.0f, 1.0f, 0.0f, 1.0f};
};


#endif //SENIORRESEARCH_LATHE_H
