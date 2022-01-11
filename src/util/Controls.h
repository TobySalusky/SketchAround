//
// Created by Tobiathan on 1/11/22.
//

#ifndef SENIORRESEARCH_CONTROLS_H
#define SENIORRESEARCH_CONTROLS_H

#include <glew.h>
#include <glfw3.h>
#include <unordered_map>
#include "../gl/Input.h"

// TODO: find overlapping keycode controls -- optimize by reversing their modifiers (if one has shift, then one with -1 won't fire for shift)

static int
    CONTROLS_SetLayerPrimary = 0,
    CONTROLS_SetLayerSecondary = 1,
    CONTROLS_SetLayerTertiary = 2,
    CONTROLS_OpenFileSaveMenu = 3,
    CONTROLS_OpenFileOpenMenu = 4,
    CONTROLS_ClearCurrentLayer = 5,
    CONTROLS_ClearAllLayers = 6,
    CONTROLS_AddLathe = 7,
    CONTROLS_AddCrossSectional = 8,
    CONTROLS_SwitchModelUp = 9,
    CONTROLS_SwitchModelDown = 10,
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

private:
    static Input* input;
    static std::unordered_map<int, KeyControl> controls;
};


void Test();

#endif //SENIORRESEARCH_CONTROLS_H
