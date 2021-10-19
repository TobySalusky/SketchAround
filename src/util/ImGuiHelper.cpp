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
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    window.SetImGuiIO(&io);

    //io.ConfigFlags |= ;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowPtr(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
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
