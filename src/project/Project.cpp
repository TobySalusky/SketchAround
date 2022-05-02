//
// Created by Tobiathan on 4/11/22.
//

#include "Project.h"
#include "../generation/Lathe.h"
#include "../misc/Serialization.h"
#include "../util/ModelObjectHelper.h"
#include <iostream>
#include <fstream>

#include <vector>
#include <boost/archive/text_iarchive.hpp>


Project::Project() {
	currentModelObject = ModelObjectHelper::CreateShareable<Lathe>();

	modelObjects = {currentModelObject->ProduceSharedPtrToSelf()};

	name = "untitled";
}

Project::Project(std::string path) {
	std::string name = path.substr(path.find_last_of('/') + 1);
	name = name.substr(0, name.find_first_of('.'));

	LOG("loading from \"%s\" -- ", path.c_str());
	Serialization serialization;

	std::ifstream ifs (path);
	boost::archive::text_iarchive ia(ifs);
	ia >> serialization;

	auto modelObjectPtrs = serialization.Deserialize();

	for (ModelObject* modelObjPtr : modelObjectPtrs) { // Load initial meshes
		modelObjects.push_back(ModelObjectHelper::CreateInitialShareableFromPtr(modelObjPtr));
	}

	currentModelObject = modelObjects[0];

	for (const auto& modelObj : modelObjects) {
		modelObj->UpdateMesh();
	}

	MakeExisting(name, path);

	LOG("load successful!");
}

void Project::MakeExisting(const std::string& projectName, const std::string& path) {
	untitled = false;

	name = projectName;
	filePath = path;
}

