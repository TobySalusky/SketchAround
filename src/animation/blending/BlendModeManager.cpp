//
// Created by Tobiathan on 11/28/21.
//

#include "BlendModeManager.h"
#include "LinearBlendMode.h"

BlendModeManager::BlendModeManager() {
    blendModes[GenNextID()] = new LinearBlendMode();
    blendModes[GenNextID()] = new SineBlendMode();
}

std::vector<int> BlendModeManager::GenAllIDs() {
    std::vector<int> vec;

    for (auto& [ID, _] : blendModes) {
        vec.emplace_back(ID);
    }

    return vec;
}
