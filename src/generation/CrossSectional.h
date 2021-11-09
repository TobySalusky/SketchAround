//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_CROSSSECTIONAL_H
#define SENIORRESEARCH_CROSSSECTIONAL_H


#include "ModelObject.h"
#include "CrossSectionTracer.h"

class CrossSectional : public ModelObject {
public:
    void HyperParameterUI() final;
    void AuxParameterUI() final;
    void ModeSetUI(Enums::DrawMode drawMode) final;
    void UpdateMesh() final;

    void InputPoints(MouseInputInfo renderInfo) override;

    void ClearAll() override;

    void ClearSingle(Enums::DrawMode drawMode) override;

    void RenderSelf2D(RenderInfo2D renderInfo) final;
    void RenderGizmos2D(RenderInfo2D renderInfo) final;

    void RenderGizmos3D(RenderInfo3D renderInfo) final;

private:
    int countPerRing = 10;

    //bool wrapStart = false, wrapEnd = false;

    std::vector<glm::vec2> boundPoints;
    std::vector<glm::vec2> centralPoints;

    std::vector<CrossSectionTracer::Segment> segments;

    glm::vec4 boundColor = {1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 centralColor = {0.0f, 0.0f, 1.0f, 1.0f};

    Mesh centralAxisMesh = {};
    Light lineLight = {{1.0f, 0.0f, 0.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};
};


#endif //SENIORRESEARCH_CROSSSECTIONAL_H
