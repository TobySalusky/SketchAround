//
// Created by Tobiathan on 2/23/22.
//

#ifndef SENIORRESEARCH_LINESTATEUNDO_H
#define SENIORRESEARCH_LINESTATEUNDO_H


#include "Undo.h"
#include "../util/Util.h"

class ModelObject;

class LineStateUndo : public Undo {
public:
    LineStateUndo(ModelObject* modelObject, Enums::DrawMode drawMode, const Vec2List& lineState);

    ModelObject* modelObject;
    Enums::DrawMode drawMode;
    Vec2List lineState;

    void Apply() override;
    static void Initialize(const UndoersInfo& info);
private:
    static Enums::DrawMode* drawModeSetter;
};

#endif //SENIORRESEARCH_LINESTATEUNDO_H
