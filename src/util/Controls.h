//
// Created by Tobiathan on 1/11/22.
//

#ifndef SENIORRESEARCH_CONTROLS_H
#define SENIORRESEARCH_CONTROLS_H

#include <glew.h>
#include <glfw3.h>
#include <unordered_map>
#include "../gl/Input.h"
#include <vector>

// TODO: find overlapping keycode controls -- optimize by reversing their modifiers (if one has shift, then one with -1 won't fire for shift)

static const int
    CONTROLS_SetLayerPrimary = 0,
    CONTROLS_SetLayerSecondary = 1,
    CONTROLS_SetLayerTertiary = 2,
    CONTROLS_Save = 3,
    CONTROLS_OpenFileOpenMenu = 4,
    CONTROLS_ClearCurrentLayer = 5,
    CONTROLS_ClearAllLayers = 6,
    CONTROLS_AddLathe = 7,
    CONTROLS_AddCrossSectional = 8,
    CONTROLS_SwitchModelUp = 9,
    CONTROLS_SwitchModelDown = 10,
    CONTROLS_Drag = 11,
    CONTROLS_Scale = 12,
    CONTROLS_Rotate = 13,
    CONTROLS_Smear = 14,
    CONTROLS_FlipHoriz = 15,
    CONTROLS_FlipVert = 16,
    CONTROLS_ReversePoints = 17,
    CONTROLS_ExitMenu = 18,
    CONTROLS_OpenControlsMenu = 19,
    CONTROLS_OpenExportMenu = 20,
    CONTROLS_ResetCamera = 21,
    CONTROLS_LockX = 22,
    CONTROLS_LockY = 23,
    CONTROLS_Erase = 24,
    CONTROLS_ScaleLocal = 25,
    CONTROLS_SetLayerQuaternary = 26,
    CONTROLS_Undo = 27,
    CONTROLS_UndoHold = 28,
    CONTROLS_Redo = 29,
    CONTROLS_RedoHold = 30,
    CONTROLS_SaveAs = 31,
	CONTROLS_ = 0,
    CONTROLS_FIN = -2
;

struct KeyControl {
    const char* name {};
    int keyCode {};
    int modifier = -1; // -1 for none
    bool press = true;
};

class Controls {
public:
    static void Initialize(Input* inputPtr);
    static bool Check(int CONTROL_CODE);

    static void GUI();

    static std::unordered_map<int, KeyControl> GenDefaultControls();

    static std::string Describe(int CONTROL_CODE);

    static void PrepUse();

private:
    static bool OverlappingCheck(int CONTROL_CODE);

    static Input* input;
    static std::unordered_map<int, KeyControl> controls;
    static std::unordered_map<int, std::vector<int>> primaryMap;

    static const char* KeyCodeToName(int keyCode);
    static int Precedence(const KeyControl& control);
};


void Test();

#endif //SENIORRESEARCH_CONTROLS_H
