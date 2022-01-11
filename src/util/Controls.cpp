//
// Created by Tobiathan on 1/11/22.
//

#include "Controls.h"
#include "../vendor/imgui/imgui.h"

Input* Controls::input = nullptr;

int SHIFT = GLFW_KEY_LEFT_SHIFT;
int COMMAND = GLFW_KEY_LEFT_SUPER;

std::unordered_map<int, KeyControl> Controls::controls = {
        {CONTROLS_SetLayerPrimary, {"Set Layer to Primary", GLFW_KEY_P}},
        {CONTROLS_SetLayerSecondary, {"Set Layer to Secondary", GLFW_KEY_Y}},
        {CONTROLS_SetLayerTertiary, {"Set Layer to Tertiary", GLFW_KEY_T}},
        {CONTROLS_OpenFileSaveMenu, {"Enter File-Save Menu", GLFW_KEY_S, COMMAND}},
        {CONTROLS_OpenFileOpenMenu, {"Enter File-Open Menu", GLFW_KEY_O, COMMAND}},
        {CONTROLS_ClearCurrentLayer, {"Clear Current Layer", GLFW_KEY_X}},
        {CONTROLS_ClearAllLayers, {"Clear All Layers", GLFW_KEY_X, SHIFT}},
        {CONTROLS_AddLathe, {"Add Lather", GLFW_KEY_N, SHIFT}},
        {CONTROLS_AddCrossSectional, {"Add CrossSectional", GLFW_KEY_N}},
        {CONTROLS_SwitchModelUp, {"Switch to Model Above", GLFW_KEY_UP, SHIFT}},
        {CONTROLS_SwitchModelDown, {"Switch to Model Below", GLFW_KEY_DOWN, SHIFT}},
};

bool Controls::Check(int CONTROL_CODE) {
    const KeyControl& control = controls[CONTROL_CODE];
    if (control.modifier != -1 && !input->Down(control.modifier)) return false;

    return control.press ? input->Pressed(control.keyCode) : input->Down(control.keyCode);
}

void Controls::Initialize(Input *inputPtr) {
    input = inputPtr;
}

void Controls::GUI() {
    for (auto& [controlCode, control] : controls) {
        ImGui::Text("%s [%i] --%i (%i)", control.name, control.keyCode, control.modifier, control.press);
    }
}

void Test() {

}