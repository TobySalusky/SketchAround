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
#include <string>


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

private:
    static ImVec4 RGB(int r, int g, int b);
};


#endif //SENIORRESEARCH_IMGUIHELPER_H
