//
// Created by Tobiathan on 2/23/22.
//

#ifndef SENIORRESEARCH_LINESTATEUNDO_H
#define SENIORRESEARCH_LINESTATEUNDO_H


#include "../Undo.h"
#include "../../util/Util.h"

class ModelObject;

class LineStateUndo : public Undo {
public:
    LineStateUndo(std::shared_ptr<ModelObject> modelObject, Enums::DrawMode drawMode, const Vec2List& lineState, float time);

    std::shared_ptr<ModelObject> modelObject;
    Enums::DrawMode drawMode;
    Vec2List lineState;
    float time;

    void Apply() override;
};

#endif //SENIORRESEARCH_LINESTATEUNDO_H
