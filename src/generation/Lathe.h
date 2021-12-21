//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_LATHE_H
#define SENIORRESEARCH_LATHE_H


#include "../util/Util.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include "ModelObject.h"

class Lathe : public ModelObject {
    using ModelObject::ModelObject;
public:

    void HyperParameterUI(const UIInfo& info) final;
    void UpdateMesh() final;
    void ModeSetUI(Enums::DrawMode& drawMode) final;

    void ClearAll() override;

    void RenderSelf2D(RenderInfo2D renderInfo) final;
    void RenderGizmos2D(RenderInfo2D renderInfo) final;

    Vec2List& GetPointsRefByMode(Enums::DrawMode drawMode) final;

    Enums::LineType LineTypeByMode(Enums::DrawMode drawMode) final;

    std::tuple<Vec3List, std::vector<GLuint>> GenMeshTuple() final;
    float scaleRadius = 1.0f, scaleZ = 1.0f, scaleY = 1.0f, leanScalar = 0.25f;

    Enums::ModelObjectType GetType() final { return Enums::LATHE; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<ModelObject>(*this);
        ar & scaleRadius;
        ar & scaleZ;
        ar & scaleY;
        ar & leanScalar;
        ar & countPerRing;
        ar & wrapStart;
        ar & wrapEnd;
        ar & plottedPoints;
        ar & graphedPointsY;
        ar & graphedPointsZ;
        ar & crossSectionPoints;
    }

    int countPerRing = 10;

    bool wrapStart = false, wrapEnd = false;

    Vec2List plottedPoints;
    Vec2List graphedPointsY;
    Vec2List graphedPointsZ;
    Vec2List crossSectionPoints;

    RGBA plotColor = {0.0f, 0.0f, 0.0f, 1.0f};
    RGBA graphColorY = {0.0f, 0.0f, 1.0f, 1.0f};
    RGBA graphColorZ = {0.0f, 1.0f, 0.0f, 1.0f};

    ModelObject* CopyInternals() final;
};


#endif //SENIORRESEARCH_LATHE_H
