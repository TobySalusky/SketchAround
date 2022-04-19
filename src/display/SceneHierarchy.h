//
// Created by Tobiathan on 4/13/22.
//

#ifndef SENIORRESEARCH_SCENEHIERARCHY_H
#define SENIORRESEARCH_SCENEHIERARCHY_H


#include "../project/Project.h"

class SceneHierarchy {
public:
	void Gui(Project& project);

	template <typename T>
	static void AddDefaultActiveModelObject();


	static void SetActiveModelObject(ModelObject* newPtr);
	static void RemoveObjectsRecursivelyByPtr(ModelObject* rootPtr, bool isRoot = true);
	static void AddPreCreatedModelObjectsRecursively(ModelObject* rootPtr);
	static bool SelectedModelObjectExists();
	static void ReSelectIfSelectedNotExisting();

	[[nodiscard]] bool FocusModeActive() const { return focusMode; }
private:
	static void RemoveModelObjectByPtr(ModelObject* ptr);
	static void AddPreCreatedModelObject(ModelObject* objPtr);
	static void DragDropModelObject();

	static void DraggableGui(Project& project);

	bool focusMode;
};


#endif //SENIORRESEARCH_SCENEHIERARCHY_H
