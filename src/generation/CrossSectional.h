//
// Created by Tobiathan on 11/7/21.
//

#ifndef SENIORRESEARCH_CROSSSECTIONAL_H
#define SENIORRESEARCH_CROSSSECTIONAL_H

#include "../util/Util.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include "ModelObject.h"
#include "CrossSectionTracer.h"

class CrossSectional : public ModelObject {
    using ModelObject::ModelObject;
public:
    void HyperParameterUI(const UIInfo& info) final;
    void UpdateMesh() final;

    void ClearAll() override;

    void RenderSelf2D(RenderInfo2D renderInfo) final;
    void RenderGizmos2D(RenderInfo2D renderInfo) final;

    void RenderGizmos3D(RenderInfo3D renderInfo) final;

    std::vector<glm::vec2>& GetPointsRefByMode(Enums::DrawMode drawMode) final;

    Enums::LineType LineTypeByMode(Enums::DrawMode drawMode) final;

    std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> GenMeshTuple(TopologyCorrector* outTopologyData = nullptr) final;

    Enums::ModelObjectType GetType() final { return Enums::CROSS_SECTIONAL; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        const int VERSION_CROSS_SECTION = 1;
        ar & boost::serialization::base_object<ModelObject>(*this);
        ar & countPerRing;
        ar & wrapStart;
        ar & wrapEnd;
        ar & boundPoints;
        ar & centralPoints;
        ar & centralAutoGenPoints;
        if (version >= VERSION_CROSS_SECTION) ar & crossSectionPoints;
        //ar & segments;
    }

    int countPerRing = 10;
    bool wrapStart = false, wrapEnd = true;

    CrossSectionTracer::CrossSectionTraceData GenTraceData();

    Vec2List boundPoints;
    Vec2List centralPoints;
    Vec2List centralAutoGenPoints;
    Vec2List crossSectionPoints;

    std::vector<CrossSectionTracer::Segment> segments;

    glm::vec4 boundColor = {1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 centralColor = {0.0f, 0.0f, 1.0f, 1.0f};
    glm::vec4 centralAutoGenColor = {0.0f, 1.0f, 0.0f, 1.0f};

    Mesh centralAxisMesh = {};
    Light lineLight = {{1.0f, 0.0f, 0.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};

    ModelObject* CopyInternals() final;
};

BOOST_CLASS_VERSION(CrossSectional, 1)



#endif //SENIORRESEARCH_CROSSSECTIONAL_H
