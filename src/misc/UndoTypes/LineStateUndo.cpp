//
// Created by Tobiathan on 2/23/22.
//

#include "LineStateUndo.h"
#include "../../generation/ModelObject.h"
#include "../../screens/MainScreen.h"
#include "../../generation/Lathe.h"

void LineStateUndo::Apply() {
	MainScreen::GetComponents().sceneHierarchy.SetActiveModelObject(modelObject.get());
    modelObject->GetAnimatorPtr()->SetTime(time); // temporal continuity!
    Vec2List& points = modelObject->GetPointsRefByMode(drawMode);
    points.clear();
    points.insert(points.end(), lineState.begin(),  lineState.end());
    modelObject->DiffPoints(drawMode);
    modelObject->UpdateMesh();
}

LineStateUndo::LineStateUndo(std::shared_ptr<ModelObject> modelObject, Enums::DrawMode drawMode, const Vec2List& lineState, float time) :
	modelObject(modelObject),
	drawMode(drawMode),
	lineState(lineState),
	time(time) {}
