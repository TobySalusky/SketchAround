//
// Created by Tobiathan on 11/19/21.
//

#ifndef SENIORRESEARCH_DISPLAY3DCONTEXT_H
#define SENIORRESEARCH_DISPLAY3DCONTEXT_H

#include "../util/Includes.h"
#include "Input.h"
#include "Camera.h"

struct Display3DContextUpdateInfo {
    Input& input;
    Camera& camera;
    Rectangle guiRect;
    bool focused;
};

class Display3DContext {
public:
    static void Update(Display3DContextUpdateInfo info);
};


#endif //SENIORRESEARCH_DISPLAY3DCONTEXT_H
