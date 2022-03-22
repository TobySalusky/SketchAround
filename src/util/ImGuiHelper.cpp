//
// Created by Tobiathan on 9/18/21.
//

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"


#include "ImGuiHelper.h"
#include "Controls.h"
#include "Util.h"

void ImGuiHelper::Initialize(GLWindow& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Regular.ttf", 18);


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




//    style.WindowMenuButtonPosition = ImGuiDir_None;
    float back = 0.1f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.PopupBorderSize = 2.0f;


    style.Colors[ImGuiCol_WindowBg] = {back, back, back, 1.0f};
    style.Colors[ImGuiCol_Separator] = COLOR_SEPARATOR;
    style.Colors[ImGuiCol_SeparatorActive] = COLOR_SEPARATOR;
    style.Colors[ImGuiCol_SeparatorHovered] = COLOR_SEPARATOR;
    style.Colors[ImGuiCol_WindowBg] = COLOR_WINDOW_BG;
    style.Colors[ImGuiCol_MenuBarBg] = COLOR_RED;

    style.Colors[ImGuiCol_TitleBg] = COLOR_SEPARATOR;
    style.Colors[ImGuiCol_TitleBgActive] = COLOR_SEPARATOR;
    style.Colors[ImGuiCol_TitleBgCollapsed] = COLOR_SEPARATOR;

//    style.Colors[ImGuiCol_NavHighlight] = COLOR_RED;
//    style.Colors[ImGuiCol_NavWindowingDimBg] = COLOR_RED;
//    style.Colors[ImGuiCol_NavWindowingHighlight] = COLOR_RED;

    style.Colors[ImGuiCol_Tab] = COLOR_BUTTON;
    style.Colors[ImGuiCol_TabActive] = COLOR_BUTTON_ACTIVE;
    style.Colors[ImGuiCol_TabHovered] = COLOR_BUTTON_HOVER;
    style.Colors[ImGuiCol_TabUnfocused] = COLOR_TAB_UNFOCUSED;
    style.Colors[ImGuiCol_TabUnfocusedActive] = COLOR_TAB_UNFOCUSED_ACTIVE;

    // >> collapsable headers
    style.Colors[ImGuiCol_Header] = COLOR_HEADER;
    style.Colors[ImGuiCol_HeaderActive] = COLOR_HEADER_HOVER;
    style.Colors[ImGuiCol_HeaderHovered] = COLOR_HEADER_ACTIVE;
    style.Colors[ImGuiCol_Button] = COLOR_BUTTON;
    style.Colors[ImGuiCol_ButtonActive] = COLOR_BUTTON_ACTIVE;
    style.Colors[ImGuiCol_ButtonHovered] = COLOR_BUTTON_HOVER;
    style.Colors[ImGuiCol_PopupBg] = COLOR_POPUP_BG;
    style.Colors[ImGuiCol_Border] = COLOR_SEPARATOR;

    style.Colors[ImGuiCol_CheckMark] = COLOR_CHECKMARK;
    style.Colors[ImGuiCol_FrameBg] = COLOR_CHECKBOX;
    style.Colors[ImGuiCol_FrameBgActive] = COLOR_CHECKBOX_ACTIVE;
    style.Colors[ImGuiCol_FrameBgHovered] = COLOR_CHECKBOX_HOVER;

    style.Colors[ImGuiCol_SliderGrab] = COLOR_DRAG;
    style.Colors[ImGuiCol_SliderGrabActive] = COLOR_DRAG_ACTIVE;
//    style.Colors[ImGuiCol_NavWindowingHighlight] = {1.0f, 0.0f, 0.0f, 1.0f};
//    style.Colors[ImGuiCol_NavHighlight] = {1.0f, 0.0f, 0.0f, 1.0f};
//    style.Colors[ImGuiCol_NavWindowingDimBg] = {1.0f, 0.0f, 0.0f, 1.0f};
//    style.
//    style.WindowPadding = {4.0f, 4.0f};
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

void ImGuiHelper::SpacedSep() {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void ImGuiHelper::DelayControlTooltip(int CONTROL_CODE) {
    if (HoverDelayTooltip()) {
        ImGui::SetTooltip("%s", Controls::Describe(CONTROL_CODE).c_str());
    }
}

void ImGuiHelper::InnerWindowBorders() {
    bool highlight = ImGui::IsWindowFocused();

    const auto Add = [](ImVec2 v1, ImVec2 v2) {
        return ImVec2(v1.x + v2.x, v1.y + v2.y);
    };

    auto pos = ImGui::GetWindowPos();
    auto dimens = ImGui::GetWindowSize();

    ImDrawList& g = *ImGui::GetWindowDrawList();
    g.PushClipRect(pos, Add(pos, dimens), false);
    if (highlight) {
        g.AddRect(Add(pos, {1.0f, 1.0f}), Add(Add(pos, dimens), {-1.0f, -1.0f}), COLOR_WINDOW_FOCUS_HIGHLIGHT, 3.0f, 0, 1.0f);
    }
    g.AddRect(pos, Add(pos, dimens), COLOR_SEPARATOR, 0.0f, 0, 1.0f);
    g.AddRect(pos, Add(pos, dimens), COLOR_SEPARATOR, 3.0f, 0, 1.0f);
    g.PopClipRect();
}

void ImGuiHelper::CreateDockSpace() {
//	ImGuiViewport const* viewport = ImGui::GetMainViewport();
//	ImGui::SetNextWindowPos(viewport->Pos);
//	ImGui::SetNextWindowSize(viewport->Size);
//	ImGui::SetNextWindowViewport(viewport->ID);
//	ImGui::SetNextWindowBgAlpha(0.0f);
//
//	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
//	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
//	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
//
////	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
////	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
////	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
////	ImGui::Begin("DockSpace Demo", p_open, window_flags);
////	ImGui::PopStyleVar(3);
//
//	ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
////	ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruDockspace;
//	ImGui::DockSpace(dockspace_id, ImVec2(400.0f, 400.0f));
}
