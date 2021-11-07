//
// Created by Tobiathan on 10/26/21.
//

#ifndef SENIORRESEARCH_MODELOBJECT_H
#define SENIORRESEARCH_MODELOBJECT_H

#include <vector>
#include <string>
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

class ModelObject {
public:
    ModelObject() = default;

    virtual void HyperParameterUI() {}
    virtual void AuxParameterUI() {}
    virtual void ModeSetUI(Enums::DrawMode drawMode) {}

    virtual void InputPoints(MouseInputInfo renderInfo) {}
    virtual void UpdateMesh() {}

    virtual void ClearAll() {}
    virtual void ClearSingle(Enums::DrawMode drawMode) {}

    void Render(RenderInfo3D renderInfo);
    void Render2D(RenderInfo2D renderInfo);

    virtual void RenderSelf2D(RenderInfo2D renderInfo) {}
    virtual void RenderGizmos2D(RenderInfo2D renderInfo) {}

    [[nodiscard]] glm::mat4 GenModelMat() const {
        return glm::translate(glm::mat4(1.0f), modelTranslation);
    }

    [[nodiscard]] bool IsVisible() const { return visible; };
    [[nodiscard]] bool* VisibilityPtr() { return &visible; };

protected:
    bool visible = true;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    glm::vec3 modelTranslation = {0.0f, 0.0f, -2.5f};
    float sampleLength = 0.1f;

    Mesh mesh{nullptr, nullptr, 0, 0};

    void ModeSet (const char* title, Enums::DrawMode newDrawMode, std::vector<glm::vec2>& clearableVec, Enums::DrawMode drawMode) {
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
};


#endif //SENIORRESEARCH_MODELOBJECT_H
