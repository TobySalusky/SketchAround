//
// Created by Tobiathan on 1/11/22.
//

#include "Controls.h"
#include "../vendor/imgui/imgui.h"
#include "Util.h"
#include <string>
#include <algorithm>


Input* Controls::input = nullptr;

const int SHIFT = GLFW_KEY_LEFT_SHIFT;
const int COMMAND = GLFW_KEY_LEFT_SUPER;

std::unordered_map<int, KeyControl> Controls::GenDefaultControls() {
    return {
            {CONTROLS_SetLayerPrimary, {"Set Layer to Primary", GLFW_KEY_1}},
            {CONTROLS_SetLayerSecondary, {"Set Layer to Secondary", GLFW_KEY_2}},
            {CONTROLS_SetLayerTertiary, {"Set Layer to Tertiary", GLFW_KEY_3}},
            {CONTROLS_SetLayerQuaternary, {"Set Layer to Quaternary", GLFW_KEY_4}},
            {CONTROLS_Save, {"Save", GLFW_KEY_S, {COMMAND}}},
            {CONTROLS_SaveAs, {"Save As", GLFW_KEY_S, {COMMAND, SHIFT}}},
            {CONTROLS_OpenFileOpenMenu, {"Enter File-Open Menu", GLFW_KEY_O, {COMMAND}}},
            {CONTROLS_ClearCurrentLayer, {"Clear Current Layer", GLFW_KEY_X, {SHIFT}}},
            {CONTROLS_ClearAllLayers, {"Clear All Layers", GLFW_KEY_X, {COMMAND}}},
            {CONTROLS_AddLathe, {"Add Lathe", GLFW_KEY_N}},
            {CONTROLS_AddCrossSectional, {"Add CrossSectional", GLFW_KEY_N, {SHIFT}}},
            {CONTROLS_SwitchModelUp, {"Switch to Model Above", GLFW_KEY_UP, {SHIFT}}},
            {CONTROLS_SwitchModelDown, {"Switch to Model Below", GLFW_KEY_DOWN, {SHIFT}}},
            {CONTROLS_Drag, {"Drag", GLFW_KEY_G}},
            {CONTROLS_Scale, {"Scale", GLFW_KEY_S}},
            {CONTROLS_Rotate, {"Rotate", GLFW_KEY_R}},
            {CONTROLS_Smear, {"Smear", GLFW_KEY_U}},
            {CONTROLS_FlipHoriz, {"Flip Horizontal", GLFW_KEY_F}},
            {CONTROLS_FlipVert, {"Flip Vertical", GLFW_KEY_J}},
            {CONTROLS_ReversePoints, {"Reverse Points", GLFW_KEY_B}},
            {CONTROLS_ExitMenu, {"Exit Menu", GLFW_KEY_ESCAPE}},
            {CONTROLS_OpenControlsMenu, {"Enter Controls Menu", GLFW_KEY_H, {SHIFT}}},
            {CONTROLS_OpenExportMenu, {"Enter Export Menu", GLFW_KEY_E, {COMMAND}}},
            {CONTROLS_ResetCamera, {"Reset Camera", GLFW_KEY_R, {SHIFT}}},
            {CONTROLS_LockX, {"Lock Transforms to X", GLFW_KEY_X}},
            {CONTROLS_LockY, {"Lock Transforms to Y", GLFW_KEY_Y}},
            {CONTROLS_Erase, {"Erase", GLFW_KEY_E, {}, false}},
            {CONTROLS_ScaleLocal, {"Scale", GLFW_KEY_S, {GLFW_KEY_L}}},
            {CONTROLS_Undo, {"Undo", GLFW_KEY_Z, {COMMAND}}},
            {CONTROLS_UndoHold, {"Undo Hold", GLFW_KEY_Z, {COMMAND}, false}},
            {CONTROLS_Redo, {"Redo", GLFW_KEY_Z, {SHIFT, COMMAND}}},
            {CONTROLS_RedoHold, {"Redo Hold", GLFW_KEY_Z, {SHIFT, COMMAND}, false}},
            {CONTROLS_Copy, {"Copy", GLFW_KEY_C, {COMMAND}}},
            {CONTROLS_Paste, {"Paste", GLFW_KEY_V, {COMMAND}}},
    };
}

std::unordered_map<int, KeyControl> Controls::controls = GenDefaultControls();
std::unordered_map<int, std::vector<int>> Controls::primaryMap = {};

void Controls::Initialize(Input *inputPtr) {
    input = inputPtr;
    PrepUse();
}

void Controls::GUI() { // TODO: fix crash with [Space] on "Add CrossSectional"
    // TODO: serialize key config
    using namespace std::string_literals;

    static KeyControl* editingControl = nullptr;

    static const char* modifiers[] = {"--", "Shift", "Command", "Control"};

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

    bool diffFlag = false;

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

        // TODO: editable mod keys

//        ImGui::SameLine();
//        int modIndex = KeyCodeToIndex(control.modifier);
//        int initModIndex = modIndex;
//        ImGui::PushItemWidth(100.0f);
//        ImGui::Combo(("##"s + control.name).c_str(), &modIndex, modifiers, IM_ARRAYSIZE(modifiers));
//        ImGui::PopItemWidth();
//        if (modIndex != initModIndex) {
//            control.modifier = IndexToKeyCode(modIndex);
//            diffFlag = true;
//        }

        ImGui::SameLine();
        ImGui::Checkbox(("Press##"s + control.name).c_str(), &control.press);
        if (ImGui::IsItemEdited()) {
            diffFlag = true;
        }

        ImGui::SameLine();
        if (ImGui::Button(("[Reset]##"s + control.name).c_str())) {
            auto defControls = GenDefaultControls();
            control = defControls[controlCode];
            diffFlag = true;
        }

        if (ImGui::BeginPopup(popupKey.c_str())) {
            ImGui::Text("%s", "Press Key (Click Anywhere to Cancel)");

            auto& controlRef = control;
            input->UsePressedCallback([&](int keyCode) {
                controlRef.keyCode = keyCode;
                diffFlag = true;
                ImGui::CloseCurrentPopup();
            });

            ImGui::EndPopup();
        }
    }

    if (diffFlag) {
        PrepUse();
    }
}

const char *Controls::KeyCodeToName(int keyCode) {
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
        case GLFW_KEY_ESCAPE:
            return "[Escape]";
        default:
            return glfwGetKeyName(keyCode, 0);
    }
}

std::string Controls::Describe(int CONTROL_CODE) {
    const KeyControl& control = controls[CONTROL_CODE];


    const char* keyNameChar = KeyCodeToName(control.keyCode);
    if (keyNameChar == nullptr) keyNameChar = "[???]";
    bool nonChar = keyNameChar[0] == '[';
    std::string keyName = keyNameChar;
    if (!nonChar) {
        std::transform(keyName.begin(), keyName.end(), keyName.begin(), ::toupper);
        keyName = "[" + keyName + "]";
    }

    const auto modifierToStr = [](int modifier){
	    if (modifier == GLFW_KEY_LEFT_SUPER) return "Command";
	    if (modifier == GLFW_KEY_LEFT_CONTROL) return "Ctrl";
	    return "Shift";
    };

    std::vector<std::string> modifierStrs;
    transform(BEG_END(control.modifiers), std::back_inserter(modifierStrs), modifierToStr);
    int i = 0;
    std::string modifierStr = std::accumulate(BEG_END(modifierStrs), std::string{}, [&](const std::string& s1, const std::string& s2){
    	if (i == 0) return s1 + s2;
		return s1 + " + " + s2;
	});

	return modifierStr + (modifierStr.empty() ? "" : " ") + keyName;
}

void Controls::PrepUse() {
    primaryMap.clear();
    for (auto& [controlCode, control] : controls) {
        int primaryKey = control.keyCode;
        if (!primaryMap.contains(primaryKey)) {
            primaryMap[primaryKey] = {controlCode};
        } else {
            primaryMap[primaryKey].emplace_back(controlCode);
        }
    }
}

int Controls::Precedence(const KeyControl &control) {
    if (!control.modifiers.empty()) {
        return ((control.press) ? 4 : 1) + (int) control.modifiers.size();
    }
    return (control.press) ? 1 : 0;
}

bool Controls::Check(int CONTROL_CODE) {
    bool triggered = OverlappingCheck(CONTROL_CODE);
    const auto& thisControl = controls[CONTROL_CODE];

    if (triggered) {
        int primaryKey = thisControl.keyCode;
        assert(primaryMap.contains(primaryKey));

        const auto& primarySharers = primaryMap[primaryKey];
        if (primarySharers.size() >= 2) {
            int selfPrecedence = Precedence(thisControl);
            for (int controlCode : primarySharers) {
                if (controlCode == CONTROL_CODE || !OverlappingCheck(controlCode)) continue;
                int otherPrecedence = Precedence(controls[controlCode]);
                if (selfPrecedence < otherPrecedence) return false;
                if (selfPrecedence == otherPrecedence) LOG("[Warning]: Same level of control precedence! (between codes: %i and %i)", CONTROL_CODE, controlCode);
            }
        }

        return true;
    }

    return false;
}

bool Controls::OverlappingCheck(int CONTROL_CODE) {
    const KeyControl& control = controls[CONTROL_CODE];

    for (int modifier : control.modifiers) {
    	if (!input->Down(modifier)) return false;
    }

    return control.press ? input->Pressed(control.keyCode) : input->Down(control.keyCode);
}
