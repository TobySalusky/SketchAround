//
// Created by Tobiathan on 9/18/21.
//

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"


#include "ImGuiHelper.h"

void ImGuiHelper::Initialize(GLWindow& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/JetBrainsMono-Regular.ttf", 18);


    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    window.SetImGuiIO(&io);

    //io.ConfigFlags |= ;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowPtr(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGuiStyle& style = ImGui::GetStyle();
    //style.Colors[ImGuiCol_Button] = {255/255.0f, 188/255.0f, 111/255.0f, 1.0f};
    float back = 0.1f;
    style.Colors[ImGuiCol_WindowBg] = {back, back, back, 1.0f};
    /*style.Colors[ImGuiCol_WindowBg] = RGB(24, 35, 24);
    style.Colors[ImGuiCol_Button] = RGB(161, 119, 77);
    style.Colors[ImGuiCol_SliderGrab] = RGB(161, 119, 77);
    style.Colors[ImGuiCol_MenuBarBg] = RGB(161, 119, 77);
    style.Colors[ImGuiCol_ButtonHovered] = RGB(255, 155, 60);
    style.Colors[ImGuiCol_ButtonActive] = RGB(255, 178, 101);*/
}

void ImGuiHelper::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiHelper::EndFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiHelper::Destroy() {
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

ImVec4 ImGuiHelper::RGB(int r, int g, int b) {
    return ImVec4((float) r / 255.0f, (float) g / 255.0f, (float) b / 255.0f, 1.0f);
}
