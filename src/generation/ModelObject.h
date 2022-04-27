//
// Created by Tobiathan on 10/26/21.
//

#ifndef SENIORRESEARCH_MODELOBJECT_H
#define SENIORRESEARCH_MODELOBJECT_H

#include <vector>
#include <string>
#include <functional>
#include "../vendor/glm/vec2.hpp"
#include "../vendor/glm/vec4.hpp"
#include "../vendor/glm/vec3.hpp"
#include "../gl/Mesh.h"
#include "../vendor/glm/ext/matrix_float4x4.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"
#include "../gl/Light.h"
#include "../gl/RenderTarget.h"
#include "../Enums.h"
#include "../gl/Mesh2D.h"
#include "../gl/Camera.h"
#include "../editing/EditingContext.h"
#include "../animation/Animator.h"
#include "../graphing/GraphView.h"
#include "TopologyCorrector.h"
#include "../misc/Undo.h"
#include "../misc/UndoTypes/LineStateUndo.h"
#include "../misc/UndoTypes/MultiUndo.h"
#include "../util/ImGuiHelper.h"
#include "../util/Controls.h"
#include <boost/serialization/access.hpp>

class Timeline;

struct TimelineGUIInfo {
    unsigned int WIDTH;
    unsigned int HEIGHT;
    Input& input;
};

struct RenderInfo3D {
    Shader3D& shader3D;
    Light& mainLight;
};

struct RenderInfo2D {
    Mesh2D& plot;
    Enums::DrawMode drawMode;
    glm::vec2 onScreen;
    GraphView& graphView;
    EditingContext& editContext;
    Input& input;
};

struct MouseInputInfo {
    Enums::DrawMode drawMode;
    glm::vec2 onScreen;
    GraphView& graphView;
};

struct EditingInfo {
    EditingContext& editContext;
    Input& input;
    Enums::DrawMode drawMode;
    glm::vec2 onScreen;
    bool graphFocused;
    GraphView& graphView;
    Rectangle guiRect;
};

struct DraggableInfo {
	std::vector<std::function<void()>> queuedMutatingFuncs;

	void EnactMutations() {
		for (const auto& func : queuedMutatingFuncs) {
			LOG("USING QUEUED!");
			func();
		}
	}
};

class Timeline;

struct UIInfo {
    Timeline& timeline;
};

struct ReSerializeInfoModelObject {
    std::vector<int> childIDs;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & childIDs;
    }
};

class ModelObject {
public:

	ModelObject() : ID(GenUniqueID()) {}
    virtual ~ModelObject() {
		LOG("ModelObject {ID: %i} destructing\n", GetID());
	} // silences boost::serialization issue

    virtual void HyperParameterUI(const UIInfo& info) {}
    void AuxParameterUI(const UIInfo& info) {

        const auto&[timeline] = info;

        ImGui::ColorEdit3("Model color", (float *) &color, ImGuiColorEditFlags_NoInputs);
        if (ImGui::CollapsingHeader("Aux")) {
            AnimatableSliderValUpdateBound("x", (float *) &modelTranslation.x, timeline);
            AnimatableSliderValUpdateBound("y", (float *) &modelTranslation.y, timeline);
            AnimatableSliderValUpdateBound("z", (float *) &modelTranslation.z, timeline);
            AnimatableSliderValUpdateBound("rot-x", (float *) &eulerAngles.x, timeline);
            AnimatableSliderValUpdateBound("rot-y", (float *) &eulerAngles.y, timeline);
            AnimatableSliderValUpdateBound("rot-z", (float *) &eulerAngles.z, timeline);
        }
    }


    virtual Enums::LineType LineTypeByMode(Enums::DrawMode drawMode) = 0;

    virtual void InputPoints(const EditingInfo& info);
    virtual void UpdateMesh() {}
    virtual std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> GenMeshTuple(TopologyCorrector* outTopologyData = nullptr) = 0;

    virtual void ClearAll() {}
    void ClearSingle(Enums::DrawMode drawMode);

    void Render3D(RenderInfo3D renderInfo);
    virtual void RenderGizmos3D(RenderInfo3D renderInfo);
    void Render2D(RenderInfo2D renderInfo);

    virtual void RenderSelf2D(RenderInfo2D renderInfo) {}
    virtual void RenderGizmos2D(RenderInfo2D renderInfo) {
        RenderTransformationGizmos(renderInfo);
    }

    [[nodiscard]] glm::mat4 GenModelMat() {
        glm::mat4 own = GenOwnModelMat();
        if (HasParent()) return parent->GenModelMat() * own;
        return own;
    }

    [[nodiscard]] Vec3 GetColor() const { return color; };
    [[nodiscard]] int GetID() const { return ID; };
    [[nodiscard]] bool IsVisible() const { return visible; };
    [[nodiscard]] bool* VisibilityPtr() { return &visible; };

    void EditMakeup(const EditingInfo& info);

    virtual std::vector<glm::vec2>& GetPointsRefByMode(Enums::DrawMode drawMode) = 0;

    void SetPoints(Enums::DrawMode drawMode, const Vec2List& newPoints);
    void FlipPoints(Enums::DrawMode drawMode, Enums::Direction direction);
    void ReversePoints(Enums::DrawMode drawMode);

    void DiffPoints(Enums::DrawMode drawMode) {
        diffed[drawMode] = true;
    }

    bool HasDiff(Enums::DrawMode drawMode) {
        return diffed[drawMode];
    }

    void UnDiffAll() {
        UnDiffPoints(Enums::MODE_PLOT);
        UnDiffPoints(Enums::MODE_GRAPH_Y);
        UnDiffPoints(Enums::MODE_GRAPH_Z);
        UnDiffPoints(Enums::MODE_CROSS_SECTION);
    }

    void UnDiffPoints(Enums::DrawMode drawMode) {
        diffed[drawMode] = false;
    }

    Animator* GetAnimatorPtr() { return &animator; }

    void AppendChild(ModelObject* child, bool maintainAbsoluteTransform = true);
    void UnParent(bool maintainAbsoluteTransform = true);
    void UnParentRaw() { parent = nullptr; }
    [[nodiscard]] bool HasParent() const { return parent != nullptr; }
    [[nodiscard]] bool IsRootNode() const { return !HasParent(); }
    bool InParentChain(ModelObject* obj);

	void DraggableGUI(DraggableInfo& info);

    static int nextUniqueID;
    static int GetCurrentNextUniqueID() { return nextUniqueID; }
    static int GenUniqueID() {
        return nextUniqueID++;
    }
    static void SetNextUniqueID(int next) { nextUniqueID = next; }
    void SetColor(Vec3 vec) { color = vec; }

    void InitializeWithWeakPtr(const std::weak_ptr<ModelObject>& weakPtr) { internalWeakPtr = weakPtr; }
    std::shared_ptr<ModelObject> ProduceSharedPtrToSelf() {
    	if (internalWeakPtr.expired()) LOG("[CRITICAL ERROR]: weak ptr in ModelObject has already been invalidated!!!");
    	return internalWeakPtr.lock();
    }

    ModelObject* CopyRecursive();

    [[nodiscard]] const std::vector<ModelObject*>& GetChildren() const { return children; };

    //ModelObject(const int& ID, const bool& visible, const Vec3& color) : ID(ID), visible(visible), color(color) {}

    void PrepSerialization() {
        reSerializeInfo = {Linq::Select<ModelObject*, int>(children, [](ModelObject* obj) { return obj->GetID(); })};
    }

    void ReSerialize(std::unordered_map<int, ModelObject*>& table) {
        const auto serializedChildren = Linq::Select<int, ModelObject*>(reSerializeInfo.childIDs, [&](int thisID) { return table[thisID]; });

        for (auto* child : serializedChildren) {
            AppendChild(child, false);
        }

        reSerializeInfo = {};
    }

    void SetPos(Vec3 vec) {
        modelTranslation =  vec;
    }

    void SetEulers(Vec3 vec) {
        eulerAngles = vec;
    }

    virtual Enums::ModelObjectType GetType() = 0;

    void SetValueByLabel(const std::string& label, const float value) { *GetFloatValuePtrByLabel(label) = value; }

    virtual float* GetFloatValuePtrByLabel(const std::string& label) {
        if (label == "x") return &modelTranslation.x;
        if (label == "y") return &modelTranslation.y;
        if (label == "z") return &modelTranslation.z;
        if (label == "rot-x") return &eulerAngles.x;
        if (label == "rot-y") return &eulerAngles.y;
        if (label == "rot-z") return &eulerAngles.z;
        if (label == "sample-length") return &sampleLength;
        return nullptr;
    }

    void TimelineDiffPos(Timeline& timeline) const;
    void TimelineDiffEulers(Timeline& timeline) const;

    Undo* GenLineStateUndo(Enums::DrawMode drawMode);

    Undo* GenAllLineStateUndo() {
        return new MultiUndo({
            GenLineStateUndo(Enums::MODE_PLOT),
            GenLineStateUndo(Enums::MODE_GRAPH_Y),
            GenLineStateUndo(Enums::MODE_GRAPH_Z),
            GenLineStateUndo(Enums::MODE_CROSS_SECTION),
        });
    }

    void ModeSet(const char* title, Enums::DrawMode newDrawMode, Enums::DrawMode& drawMode, int controlCode) {

        if (ImGuiHelper::ChoiceButton(title, drawMode == newDrawMode) || Controls::Check(controlCode)) drawMode = newDrawMode;
	    ImGuiHelper::DelayControlTooltip(controlCode);

        ImGui::SameLine();

        if (ImGuiHelper::TransparButton((std::string("X##") + title).c_str())) {
            Undos::Add(GenLineStateUndo(newDrawMode));
            GetPointsRefByMode(newDrawMode).clear();
            DiffPoints(newDrawMode);
            UpdateMesh();
        }
    }

protected:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        const unsigned int VERSION_SAMPLE_LENGTH = 1;

        ar & reSerializeInfo;
        ar & ID;
        ar & visible;
        ar & color;
        ar & modelTranslation;
        ar & eulerAngles;
        if (version >= VERSION_SAMPLE_LENGTH) ar & sampleLength;
        ar & animator;
    }

    static void RenderTransformationGizmos(RenderInfo2D info);

    void AnimatableSliderValUpdateBound(const std::string& label, float* ptr, Timeline& timeline, float min = NAN, float max = NAN, float vSpeed = 0.025f);

    virtual ModelObject* CopyInternals() = 0;

    ReSerializeInfoModelObject reSerializeInfo = {};
    int ID;
    bool visible = true;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    glm::vec3 modelTranslation = {0.0f, 0.0f, 0.0f};
    glm::vec3 eulerAngles = {0.0f, 0.0f, 0.0f};
    float sampleLength = 0.1f;
    std::weak_ptr<ModelObject> internalWeakPtr;

    bool diffed[3] {};

    Animator animator = {};

    Mesh mesh{nullptr, nullptr, 0, 0};

    ModelObject* parent = nullptr;
    std::vector<ModelObject*> children;

    [[nodiscard]] glm::mat4 GenOwnModelMat() const {
        glm::mat4 rot = glm::toMat4(glm::quat(eulerAngles));
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), modelTranslation);
        return trans * rot;
    }

    static void RenderCanvasLines(const Vec2List& points, Vec4 color, Mesh2D& plot) {
        if (points.size() == 1) {
            plot.AddPolygonOutline(points[0], 0.0001f, 10, color, 0.01f);
        } else if (points.size() > 1) {
            plot.AddLines(points, color);
        }
    };

    static void FunctionalAngleGizmo(RenderInfo2D renderInfo, const std::vector<glm::vec2>& points);
    void EditCurrentLines(const EditingInfo& info);

    void PersistModelMat(MatrixComponents initFull, MatrixComponents parentMat);
};

namespace boost::serialization {

    template<class Archive>
    void serialize(Archive & ar, Vec2& vec, const unsigned int version)
    {
        ar & vec.x;
        ar & vec.y;
    }

    template<class Archive>
    void serialize(Archive & ar, Vec3& vec, const unsigned int version)
    {
        ar & vec.x;
        ar & vec.y;
        ar & vec.z;
    }

    template<class Archive>
    void serialize(Archive & ar, Vec4& vec, const unsigned int version)
    {
        ar & vec.x;
        ar & vec.y;
        ar & vec.z;
        ar & vec.w;
    }
}

BOOST_CLASS_VERSION(ModelObject, 1)

#endif //SENIORRESEARCH_MODELOBJECT_H
