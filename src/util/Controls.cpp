//
// Created by Tobiathan on 1/11/22.
//

#include "Controls.h"
#include "../vendor/imgui/imgui.h"
#include <string>
#include <algorithm>


Input* Controls::input = nullptr;

int SHIFT = GLFW_KEY_LEFT_SHIFT;
int COMMAND = GLFW_KEY_LEFT_SUPER;


std::unordered_map<int, KeyControl> Controls::GenDefaultControls() {
    return {
            {CONTROLS_SetLayerPrimary, {"Set Layer to Primary", GLFW_KEY_P}},
            {CONTROLS_SetLayerSecondary, {"Set Layer to Secondary", GLFW_KEY_Y}},
            {CONTROLS_SetLayerTertiary, {"Set Layer to Tertiary", GLFW_KEY_T}},
            {CONTROLS_OpenFileSaveMenu, {"Enter File-Save Menu", GLFW_KEY_S, COMMAND}},
            {CONTROLS_OpenFileOpenMenu, {"Enter File-Open Menu", GLFW_KEY_O, COMMAND}},
            {CONTROLS_ClearCurrentLayer, {"Clear Current Layer", GLFW_KEY_X}},
            {CONTROLS_ClearAllLayers, {"Clear All Layers", GLFW_KEY_X, SHIFT}},
            {CONTROLS_AddLathe, {"Add Lathe", GLFW_KEY_N, SHIFT}},
            {CONTROLS_AddCrossSectional, {"Add CrossSectional", GLFW_KEY_N}},
            {CONTROLS_SwitchModelUp, {"Switch to Model Above", GLFW_KEY_UP, SHIFT}},
            {CONTROLS_SwitchModelDown, {"Switch to Model Below", GLFW_KEY_DOWN, SHIFT}},
            {CONTROLS_Drag, {"Drag", GLFW_KEY_G}},
            {CONTROLS_Scale, {"Scale", GLFW_KEY_S}},
            {CONTROLS_Rotate, {"Rotate", GLFW_KEY_R}},
            {CONTROLS_Smear, {"Smear", GLFW_KEY_U}},
            {CONTROLS_FlipHoriz, {"Flip Horizontal", GLFW_KEY_F}},
            {CONTROLS_FlipVert, {"Flip Vertical", GLFW_KEY_J}},
            {CONTROLS_ReversePoints, {"Reverse Points", GLFW_KEY_B}},
    };
}

std::unordered_map<int, KeyControl> Controls::controls = GenDefaultControls();

bool Controls::Check(int CONTROL_CODE) {
    const KeyControl& control = controls[CONTROL_CODE];
    if (control.modifier != -1 && !input->Down(control.modifier)) return false;

    return control.press ? input->Pressed(control.keyCode) : input->Down(control.keyCode);
}

void Controls::Initialize(Input *inputPtr) {
    input = inputPtr;
}

void Controls::GUI() { // TODO: fix crash with [Space] on "Add CrossSectional"
    // TODO: serialize key config
    using namespace std::string_literals;

    static KeyControl* editingControl = nullptr;

    static const char* modifiers[] = {"--", "Shift", "Command", "Control"};
    const auto KeyCodeToName = [](int keyCode) {
        switch (keyCode) {
            case GLFW_KEY_LEFT_SHIFT:
                return "[Shift]";
            case GLFW_KEY_LEFT_CONTROL:
                return "[Ctrl]";
            case GLFW_KEY_LEFT_SUPER:
                return "[Command]";
            case GLFW_KEY_LEFT_ALT:
                return "[Alt]";
            case GLFW_KEY_UP:
                return "[Up]";
            case GLFW_KEY_DOWN:
                return "[Down]";
            case GLFW_KEY_LEFT:
                return "[Left]";
            case GLFW_KEY_RIGHT:
                return "[Right]";
            case GLFW_KEY_SPACE:
                return "[Space]";
            case GLFW_KEY_ENTER:
                return "[Enter]";
        }
        return glfwGetKeyName(keyCode, 0);
    };
    const auto KeyCodeToIndex = [](int keyCode) {
        switch (keyCode) {
            case GLFW_KEY_LEFT_SHIFT:
                return 1;
            case GLFW_KEY_LEFT_SUPER:
                return 2;
            case GLFW_KEY_LEFT_CONTROL:
                return 3;
            default:
                return 0;
        }
    };
    const auto IndexToKeyCode = [](int index) {
        switch (index) {
            case 1:
                return GLFW_KEY_LEFT_SHIFT;
            case 2:
                return GLFW_KEY_LEFT_SUPER;
            case 3:
                return GLFW_KEY_LEFT_CONTROL;
            default:
                return 0;
        }
    };

    for (auto& [controlCode, control] : controls) {
        ImGui::Text("%25s", control.name);

        ImGui::SameLine();

        std::string popupKey = "Record_Key-"s + control.name;

        const char* keyNameChar = KeyCodeToName(control.keyCode);
        if (keyNameChar == nullptr) keyNameChar = "[???]";
        bool nonChar = keyNameChar[0] == '[';
        std::string keyName = keyNameChar;
        if (!nonChar) std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);

        if (ImGui::Button((keyName + "##" + control.name).c_str(), {140.0f, 25.0f})) {
            editingControl = &control;
            ImGui::OpenPopup(popupKey.c_str());
        }

        ImGui::SameLine();
        int modIndex = KeyCodeToIndex(control.modifier);
        int initModIndex = modIndex;
        ImGui::PushItemWidth(100.0f);
        ImGui::Combo(("##"s + control.name).c_str(), &modIndex, modifiers, IM_ARRAYSIZE(modifiers));
        ImGui::PopItemWidth();
        if (modIndex != initModIndex) control.modifier = IndexToKeyCode(modIndex);

        ImGui::SameLine();
        ImGui::Checkbox(("Press##"s + control.name).c_str(), &control.press);

        ImGui::SameLine();
        if (ImGui::Button(("[Reset]##"s + control.name).c_str())) {
            auto defControls = GenDefaultControls();
            control = defControls[controlCode];
        }

        if (ImGui::BeginPopup(popupKey.c_str())) {
            ImGui::Text("%s", "Press Key (Click Anywhere to Cancel)");

            auto& controlRef = control;
            input->UsePressedCallback([&](int keyCode) {
                controlRef.keyCode = keyCode;
                ImGui::CloseCurrentPopup();
            });

            ImGui::EndPopup();
        }
    }
}

void Test() {

}