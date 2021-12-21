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
};

struct MouseInputInfo {
    Enums::DrawMode drawMode;
    glm::vec2 onScreen;
    Camera& camera;
};

struct EditingInfo {
    EditingContext& editContext;
    Input& input;
    Enums::DrawMode drawMode;
    glm::vec2 onScreen;
    Camera& camera;
    bool graphFocused;
};

class Timeline;

struct UIInfo {
    Timeline& timeline;
};

struct DraggableUIInfo;

class ModelObject {
public:
    ModelObject() : ID(GenUniqueID()) {}

    virtual void HyperParameterUI(const UIInfo& info) {}
    void AuxParameterUI(const UIInfo& info) {
        if (ImGui::CollapsingHeader("Aux")) {
            ImGui::ColorEdit3("model-color", (float *) &color);
            ImGui::SliderFloat3("translate", (float *) &modelTranslation, -5.f, 5.f);
            ImGui::SliderFloat3("rotation", (float *) &eulerAngles, 0, (float) M_PI * 2.0f);
        }
    }
    virtual void ModeSetUI(Enums::DrawMode& drawMode) {}

    virtual Enums::LineType LineTypeByMode(Enums::DrawMode drawMode) = 0;

    virtual void InputPoints(MouseInputInfo renderInfo);
    virtual void UpdateMesh() {}
    virtual std::tuple<std::vector<glm::vec3>, std::vector<GLuint>> GenMeshTuple() = 0;

    virtual void ClearAll() {}
    void ClearSingle(Enums::DrawMode drawMode);

    void Render3D(RenderInfo3D renderInfo);
    virtual void RenderGizmos3D(RenderInfo3D renderInfo);
    void Render2D(RenderInfo2D renderInfo);

    virtual void RenderSelf2D(RenderInfo2D renderInfo) {}
    virtual void RenderGizmos2D(RenderInfo2D renderInfo) {}

    [[nodiscard]] glm::mat4 GenModelMat() {
        glm::mat4 own = GenOwnModelMat();
        if (HasParent()) return parent->GenModelMat() * GenOwnModelMat();
        return GenOwnModelMat();
    }

    [[nodiscard]] bool IsVisible() const { return visible; };
    [[nodiscard]] bool* VisibilityPtr() { return &visible; };

    void EditMakeup(EditingInfo info);

    virtual std::vector<glm::vec2>& GetPointsRefByMode(Enums::DrawMode drawMode) = 0;

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

    void AppendChild(ModelObject* child);
    void UnParent();
    [[nodiscard]] bool HasParent() const { return parent != nullptr; }
    bool InParentChain(ModelObject* obj);

    void DraggableGUI(const DraggableUIInfo& draggableUIInfo);

    static int GenUniqueID() {
        static int uniqueID = 0;
        return uniqueID++;
    }

    ModelObject* CopyRecursive();

    [[nodiscard]] const std::vector<ModelObject*>& GetChildren() const { return children; };

protected:
    virtual ModelObject* CopyInternals() = 0;

    int ID;
    bool visible = true;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    glm::vec3 modelTranslation = {0.0f, 0.0f, 0.0f};
    glm::vec3 eulerAngles = {0.0f, 0.0f, 0.0f};
    float sampleLength = 0.1f;

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

    void ModeSet (const char* title, Enums::DrawMode newDrawMode, std::vector<glm::vec2>& clearableVec, Enums::DrawMode& drawMode) {
        if (drawMode == newDrawMode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
        else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});
        if (ImGui::Button(title)) drawMode = newDrawMode;
        ImGui::PopStyleColor(1);

        ImGui::SameLine();
        if (ImGui::Button((std::string("X##") + title).c_str())) {
            clearableVec.clear();
            UpdateMesh();
        }
    };

    static void FunctionalAngleGizmo(RenderInfo2D renderInfo, const std::vector<glm::vec2>& points);
    void EditCurrentLines(EditingInfo info);
};

struct DraggableUIInfo {
    std::function<bool(ModelObject*)> isSelected;
    std::function<void(ModelObject*)> select;
    std::function<void(ModelObject*)> addObj;
    std::function<void(ModelObject*)> deleteObj;
};

#endif //SENIORRESEARCH_MODELOBJECT_H
