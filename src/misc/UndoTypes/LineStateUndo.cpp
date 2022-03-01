//
// Created by Tobiathan on 2/23/22.
//

#include "LineStateUndo.h"
#include "../../generation/ModelObject.h"

Enums::DrawMode* LineStateUndo::drawModeSetter = nullptr;
std::function<void(ModelObject*)> LineStateUndo::modelObjectSetter = {};

void LineStateUndo::Initialize(const UndoersInfo &info) {
    drawModeSetter = info.drawModeSetter;
    modelObjectSetter = info.modelObjectSetter;
}

void LineStateUndo::Apply() {
    modelObjectSetter(modelObject);
    modelObject->GetAnimatorPtr()->SetTime(time); // temporal continuity!
    Vec2List& points = modelObject->GetPointsRefByMode(drawMode);
    points.clear();
    points.insert(points.end(), lineState.begin(),  lineState.end());
    modelObject->DiffPoints(drawMode);
    modelObject->UpdateMesh();
}

LineStateUndo::LineStateUndo(ModelObject* modelObject, Enums::DrawMode drawMode, const Vec2List& lineState, float time) {
    this->modelObject = modelObject;
    this->drawMode = drawMode;
    this->lineState = lineState;
    this->time = time;
}
