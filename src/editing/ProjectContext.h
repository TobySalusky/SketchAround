//
// Created by Tobiathan on 3/30/22.
//

#ifndef SENIORRESEARCH_PROJECTCONTEXT_H
#define SENIORRESEARCH_PROJECTCONTEXT_H


class ProjectContext {
public:
	[[nodiscard]] virtual const char* GetName() const = 0;
	virtual bool SaveIsNew() const = 0;
};


#endif //SENIORRESEARCH_PROJECTCONTEXT_H
