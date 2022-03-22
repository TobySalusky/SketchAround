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

#define TRIP(a) a, a, a

#define COLOR_INVIS ImColor(0.0f, 0.0f, 0.0f, 0.0f)
#define COLOR_INVIS_HOVER_HIGHLIGHT ImColor(1.0f, 1.0f, 1.0f, 0.15f)
#define COLOR_INVIS_ACTIVE_HIGHLIGHT ImColor(1.0f, 1.0f, 1.0f, 0.3f)
#define COLOR_SEPARATOR ImColor(TRIP(0.05f))
#define COLOR_WINDOW_FOCUS_HIGHLIGHT ImColor(255, 122, 0)
#define COLOR_WINDOW_BG ImColor(TRIP(0.20f))
#define COLOR_RED ImColor(1.0f, 0.0f, 0.0f)
#define COLOR_BUTTON ImColor(TRIP(0.35f))
#define COLOR_BUTTON_HOVER ImColor(TRIP(0.4f))
#define COLOR_BUTTON_ACTIVE ImColor(TRIP(0.5f))
#define COLOR_CHECKMARK ImColor(TRIP(0))
#define COLOR_CHECKBOX ImColor(TRIP(0.5f))
#define COLOR_CHECKBOX_HOVER ImColor(0.65f, 0.6f, 0.55f)
#define COLOR_CHECKBOX_ACTIVE ImColor(0.75f, 0.7f, 0.65f)
#define COLOR_TAB_UNFOCUSED ImColor(0.1f, 0.1f, 0.1f)
#define COLOR_TAB_UNFOCUSED_ACTIVE ImColor(0.25f, 0.25f, 0.25f)
#define COLOR_POPUP_BG ImColor(0.1f, 0.1f, 0.1f, 0.95f)
#define COLOR_DRAG ImColor(TRIP(0.05f))
#define COLOR_DRAG_ACTIVE ImColor(TRIP(0.15f))
#define COLOR_DRAG_TEXT ImColor(255, 122, 0)
#define COLOR_ANIMATE_KEYFRAME_JUMPER_TEXT ImColor(43, 161, 255)
#define COLOR_HEADER ImColor(TRIP(0.08f))
#define COLOR_HEADER_HOVER ImColor(TRIP(0.10f))
#define COLOR_HEADER_ACTIVE ImColor(TRIP(0.12f))

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

    static void CreateDockSpace();

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

    static void ValDrag(const char* label, float* fPtr, float vSpeed = 0.025f) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(COLOR_INVIS));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(COLOR_INVIS_HOVER_HIGHLIGHT));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(COLOR_INVIS_ACTIVE_HIGHLIGHT));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(COLOR_DRAG_TEXT));
        ImGui::SetNextItemWidth(80.0f);
//        std::string idStr = "##" + label;
        ImGui::DragFloat((std::string("##") + label).c_str(), fPtr, vSpeed);
        ImGui::PopStyleColor(4);

        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }
    }

private:
    static ImVec4 RGB(int r, int g, int b);
};


#endif //SENIORRESEARCH_IMGUIHELPER_H
