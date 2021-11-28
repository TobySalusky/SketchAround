//
// Created by Tobiathan on 11/28/21.
//

#include "BlendModeManager.h"
#include "LinearBlendMode.h"

BlendModeManager::BlendModeManager() {
    blendModes[GenNextID()] = new LinearBlendMode();
    blendModes[GenNextID()] = new SineBlendMode();
}
