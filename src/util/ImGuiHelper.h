//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_IMGUIHELPER_H
#define SENIORRESEARCH_IMGUIHELPER_H


#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

#include "../gl/GLWindow.h"
#include "Rectangle.h"
#include "../vendor/imgui/imgui_internal.h"
#include <string>

#define COLOR_SEPARATOR ImColor(0.05f, 0.05f, 0.05f)
#define COLOR_WINDOW_FOCUS_HIGHLIGHT ImColor(255, 122, 0)
#define COLOR_WINDOW_BG ImColor(0.15f, 0.15f, 0.15f)
#define COLOR_RED ImColor(1.0f, 0.0f, 0.0f)
#define COLOR_BUTTON ImColor(0.3f, 0.3f, 0.3f)
#define COLOR_BUTTON_HOVER ImColor(0.4f, 0.4f, 0.4f)
#define COLOR_BUTTON_ACTIVE ImColor(0.5f, 0.5f, 0.5f)
#define COLOR_CHECKBOX ImColor(0.5f, 0.5f, 0.5f)
#define COLOR_CHECKBOX_HOVER ImColor(0.6f, 0.6f, 0.6f)
#define COLOR_CHECKBOX_ACTIVE ImColor(0.7f, 0.7f, 0.7f)

struct TimedPopup {

    float maxTime = 1.0f;

    void Open(const std::string& text) {
        this->text = text;
        timeLeft = maxTime;
    }

    void Update(float deltaTime) {
        timeLeft -= deltaTime;
    }

    void RenderGUI() {
        if (ImGui::BeginPopup((text + "-popup").c_str())) {

            ImGui::Text("%s", text.c_str());

            if (timeLeft <= 0.0f) ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        } else if (timeLeft > 0.0f) {
            ImGui::OpenPopup((text + "-popup").c_str());
        }
    }

private:
    std::string text;
    float timeLeft;
};

class ImGuiHelper {
public:
    static void BeginFrame();
    static void EndFrame();

    static void Destroy();

    static void Initialize(GLWindow &window);

    static Rectangle ItemRect() {
        return {ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, ImGui::GetItemRectSize().x, ImGui::GetItemRectSize().y};
    }

    // assumes equal amount of padding on top/and & left/right
    static Rectangle ItemRectRemovePadding(float paddingLeft, float paddingTop) {
        return {ImGui::GetItemRectMin().x + paddingLeft, ImGui::GetItemRectMin().y + paddingTop, ImGui::GetItemRectSize().x - paddingLeft * 2, ImGui::GetItemRectSize().y - paddingTop * 2};
    }

    static void SpacedSep();

    static bool HoverDelayTooltip(float delay = 0.65f) {
        return ImGui::IsItemHovered() && GImGui->HoveredIdTimer > delay;
    }

    static void DelayControlTooltip(int CONTROL_CODE);

    static void InnerWindowBorders();

    static void BeginComponentWindow(const char* label) {
        ImGui::Begin(label, nullptr, ImGuiWindowFlags_NoTitleBar);
    }

private:
    static ImVec4 RGB(int r, int g, int b);
};


#endif //SENIORRESEARCH_IMGUIHELPER_H
