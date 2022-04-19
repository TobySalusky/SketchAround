#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-make-member-function-const"
//
// Created by Tobiathan on 4/11/22.
//

#ifndef SENIORRESEARCH_PROJECT_H
#define SENIORRESEARCH_PROJECT_H

#include <vector>
#include <string>
#include <filesystem>
#include <boost/archive/text_iarchive.hpp>
#include "../generation/ModelObject.h"

class Project {
public:
	Project();
	explicit Project(std::string path); // TODO:

	void SetModelObjects(const std::vector<std::shared_ptr<ModelObject>>& newModelObjects) { modelObjects = newModelObjects; }
	void SetCurrentModelObject(const std::shared_ptr<ModelObject>& newCurrentModelObjects) {
		currentModelObject = newCurrentModelObjects;
	}
	[[nodiscard]] const std::vector<std::shared_ptr<ModelObject>>& GetModelObjects() const { return modelObjects; }
	[[nodiscard]] const std::shared_ptr<ModelObject>& GetCurrentModelObject() const { return currentModelObject; }
	[[nodiscard]] bool IsUntitled() const { return untitled; }
	[[nodiscard]] std::string GetName() const { return name; }
	[[nodiscard]] std::string GetPath() const { return filePath; }
	[[nodiscard]] const std::string& GenWindowTitle() const { return name; }

	void UpdateCurrentMesh() { GetCurrentModelObject()->UpdateMesh(); }

	void MakeExisting(const std::string& projectName, const std::string& path);
private:
	std::vector<std::shared_ptr<ModelObject>> modelObjects;
	std::shared_ptr<ModelObject> currentModelObject;

	bool untitled = true;
	std::string name;
	std::string filePath;
};


#endif //SENIORRESEARCH_PROJECT_H

#pragma clang diagnostic pop