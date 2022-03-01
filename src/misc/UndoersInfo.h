//
// Created by Tobiathan on 2/23/22.
//

#ifndef SENIORRESEARCH_UNDOERSINFO_H
#define SENIORRESEARCH_UNDOERSINFO_H


#include "../Enums.h"
#include <functional>

class ModelObject;

struct UndoersInfo {
    Enums::DrawMode* drawModeSetter;
    std::function<void(ModelObject*)> modelObjectSetter;
};


#endif //SENIORRESEARCH_UNDOERSINFO_H
