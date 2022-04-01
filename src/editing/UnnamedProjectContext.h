//
// Created by Tobiathan on 3/30/22.
//

#ifndef SENIORRESEARCH_UNNAMEDPROJECTCONTEXT_H
#define SENIORRESEARCH_UNNAMEDPROJECTCONTEXT_H


#include "ProjectContext.h"

class UnnamedProjectContext : public ProjectContext {
public:
	[[nodiscard]] const char* GetName() const override {
		return "untitled";
	}

	bool SaveIsNew() const override;
};


#endif //SENIORRESEARCH_UNNAMEDPROJECTCONTEXT_H
