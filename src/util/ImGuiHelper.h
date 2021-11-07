//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_IMGUIHELPER_H
#define SENIORRESEARCH_IMGUIHELPER_H

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

#include "../gl/GLWindow.h"


class ImGuiHelper {
public:
    static void BeginFrame();
    static void EndFrame();

    static void Destroy();

    static void Initialize(GLWindow &window);

private:
    static ImVec4 RGB(int r, int g, int b);
};


#endif //SENIORRESEARCH_IMGUIHELPER_H
