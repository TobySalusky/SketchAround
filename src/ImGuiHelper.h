//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_IMGUIHELPER_H
#define SENIORRESEARCH_IMGUIHELPER_H

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "GLWindow.h"


class ImGuiHelper {
public:
    static void Initialize(const GLWindow& window);
    static void BeginFrame();
    static void EndFrame();

    static void Destroy();
};


#endif //SENIORRESEARCH_IMGUIHELPER_H
