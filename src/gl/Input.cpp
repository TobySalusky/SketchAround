//
// Created by Tobiathan on 9/18/21.
//

#include "Input.h"

void Input::EndUpdate() {
    for (int i = 0; i < 1024; ++i) {
        keysPressed[i] = false;
        keysUnpressed[i] = false;
    }
    mouseDiffX = 0;
    mouseDiffY = 0;

    mousePressed = false;
    mouseUnpressed = false;
}

void Input::SetKey(int key, bool val) {
    if (val) {
        keysPressed[key] = true;
    } else {
        keysUnpressed[key] = true;
    }
    keys[key] = val;
}
