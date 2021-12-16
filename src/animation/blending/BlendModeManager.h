//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_BLENDMODEMANAGER_H
#define SENIORRESEARCH_BLENDMODEMANAGER_H

#include <unordered_map>
#include "BlendMode.h"
#include "SineBlendMode.h"
#include <vector>

// FIXME: NEVER CLEANED UP (perhaps use unique_ptr?)
class BlendModeManager {
public:
    BlendModeManager();

    BlendMode* Get(int ID) { return blendModes[ID]; }

    void Add(BlendMode* newBlendMode) { blendModes[GenNextID()] = newBlendMode; }

    std::vector<int> GenAllIDs();

    [[nodiscard]] int GetNextID() const { return nextID; }

private:
    int GenNextID() { return nextID++; }

    int nextID = 0;
    std::unordered_map<int, BlendMode*> blendModes;
};


#endif //SENIORRESEARCH_BLENDMODEMANAGER_H
