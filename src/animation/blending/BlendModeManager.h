//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_BLENDMODEMANAGER_H
#define SENIORRESEARCH_BLENDMODEMANAGER_H

#include <unordered_map>
#include "BlendMode.h"
#include "SineBlendMode.h"

// FIXME: NEVER CLEANED UP (perhaps use unique_ptr?)
class BlendModeManager {
public:
    BlendModeManager();

    BlendMode* Get(int ID) { return blendModes[ID]; }

    void Add(BlendMode* newBlendMode) { blendModes[GenNextID()] = newBlendMode; }

private:
    int GenNextID() { return nextID++; }

    int nextID = 0;
    std::unordered_map<int, BlendMode*> blendModes;
};


#endif //SENIORRESEARCH_BLENDMODEMANAGER_H
