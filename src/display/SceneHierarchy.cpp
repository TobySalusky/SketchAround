//
// Created by Tobiathan on 4/13/22.
//

#include "SceneHierarchy.h"
#include "../util/Controls.h"
#include "../util/ModelObjectHelper.h"
#include "../generation/ModelObject.h"
#include "../generation/CrossSectional.h"
#include "../generation/Lathe.h"
#include "../program/Program.h"
#include "../screens/MainScreen.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <numeric>

void SceneHierarchy::Gui(Project& project) {
	// Model Instantiation Window
	ImGui::Begin("Models");
	if (ImGui::Button("+##Lathe") || Controls::Check(CONTROLS_AddLathe)) {
		AddDefaultActiveModelObject<Lathe>();
	}
	ImGuiHelper::DelayControlTooltip(CONTROLS_AddLathe);

	ImGui::SameLine();
	if (ImGui::Button("+##CrossSectional") || Controls::Check(CONTROLS_AddCrossSectional)) {
		AddDefaultActiveModelObject<CrossSectional>();
	}
	ImGuiHelper::DelayControlTooltip(CONTROLS_AddCrossSectional);

	ImGui::SameLine();
	ImGui::Checkbox("Focus", &focusMode);

	DraggableGui(project);

	DragDropModelObject();

	ImGuiHelper::InnerWindowBorders();
	ImGui::End();
}

void SceneHierarchy::DragDropModelObject() {
	ImGui::Dummy({ImGui::GetWindowContentRegionWidth(), fmax(40.0f, ImGui::GetContentRegionAvail().y)});
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelObjectDrag"))
		{
			IM_ASSERT(payload->DataSize == sizeof(ModelObject*));
			ModelObject* draggedObj = *(ModelObject**)payload->Data;
			draggedObj->UnParent();
		}
		ImGui::EndDragDropTarget();
	}
}

void SceneHierarchy::SetActiveModelObject(ModelObject* newPtr) {
	for (const auto& obj : Program::GetProject().GetModelObjects()) {
		if (obj.get() == newPtr) {
			Program::GetProject().SetCurrentModelObject(obj);
			Program::GetProject().UpdateCurrentMesh();
			MainScreen::GetComponents().timeline.OnActiveModelObjectChange();
			return;
		}
	}

	LOG("[Error]: The new selected model object was not found within modelObjects!!");
}

template<typename T>
void SceneHierarchy::AddDefaultActiveModelObject() {
	auto& project = Program::GetProject();
	auto newModelObjects = project.GetModelObjects();
	auto newObj = ModelObjectHelper::CreateShareable<T>();
	newModelObjects.push_back(newObj);
	project.SetModelObjects(newModelObjects);
	SetActiveModelObject(newObj.get());
}

void SceneHierarchy::AddPreCreatedModelObject(ModelObject* objPtr) {
	auto& project = Program::GetProject();
	auto newModelObjects = project.GetModelObjects();
	auto newObj = ModelObjectHelper::CreateInitialShareableFromPtr(objPtr);
	newModelObjects.push_back(newObj);
	project.SetModelObjects(newModelObjects);
}

void SceneHierarchy::RemoveModelObjectByPtr(ModelObject *ptr) {

	auto& project = Program::GetProject();

	auto initModelObjects = project.GetModelObjects();
	std::vector<std::shared_ptr<ModelObject>> newModelObjects;
	std::remove_copy_if(BEG_END(initModelObjects),
	             std::back_inserter(newModelObjects),
	             [=](const std::shared_ptr<ModelObject>& shared) { return shared.get() == ptr; });

	project.SetModelObjects(newModelObjects);
}

void SceneHierarchy::AddPreCreatedModelObjectsRecursively(ModelObject *rootPtr) {
	for (ModelObject* childPtr : rootPtr->GetChildren()) {
		AddPreCreatedModelObjectsRecursively(childPtr);
	}
	AddPreCreatedModelObject(rootPtr);
}

void SceneHierarchy::RemoveObjectsRecursivelyByPtr(ModelObject *rootPtr, bool isRoot) {
	for (ModelObject* childPtr : rootPtr->GetChildren()) {
		RemoveObjectsRecursivelyByPtr(childPtr, false);
	}
	RemoveModelObjectByPtr(rootPtr);

	if (isRoot) rootPtr->UnParent(false);
}

bool SceneHierarchy::SelectedModelObjectExists() {
	Project& project = Program::GetProject();
	const auto& modelObjects = project.GetModelObjects();
	const auto& selectedPtr = project.GetCurrentModelObject().get();

	return std::any_of(BEG_END(modelObjects), [selectedPtr](const auto& shared) { return shared.get() == selectedPtr; });
}

void SceneHierarchy::ReSelectIfSelectedNotExisting() {
	if (SelectedModelObjectExists()) return;

	Project& project = Program::GetProject();

	SetActiveModelObject(project.GetModelObjects()[0].get());
}

void SceneHierarchy::DraggableGui(Project &project) {
	DraggableInfo draggableContext;
	for (const auto& currModelObject : project.GetModelObjects()) {
		// Only run on root nodes b/c recursively renders children
		if (currModelObject->IsRootNode()) currModelObject->DraggableGUI(draggableContext);
	}
	draggableContext.EnactMutations();
}
