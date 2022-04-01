//
// Created by Tobiathan on 3/30/22.
//

#ifndef SENIORRESEARCH_EXISTINGPROJECTCONTEXT_H
#define SENIORRESEARCH_EXISTINGPROJECTCONTEXT_H


#include "ProjectContext.h"
#include <string>

class ExistingProjectContext : public ProjectContext {
public:
	std::string name;
	std::string path;

	ExistingProjectContext(std::string& name, std::string& path) : name(name), path(path) {}

	[[nodiscard]] const char* GetName() const override {
		return name.c_str();
	}

	bool SaveIsNew() const override;
};


#endif //SENIORRESEARCH_EXISTINGPROJECTCONTEXT_H
