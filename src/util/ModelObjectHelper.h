//
// Created by Tobiathan on 4/15/22.
//

#ifndef SENIORRESEARCH_MODELOBJECTHELPER_H
#define SENIORRESEARCH_MODELOBJECTHELPER_H

#include "../generation/ModelObject.h"

class ModelObjectHelper {
public:
	template <typename T>
	static std::shared_ptr<ModelObject> CreateShareable();


	static std::shared_ptr<ModelObject> CreateInitialShareableFromPtr(ModelObject* objPtr);


};

template<typename T>
std::shared_ptr<ModelObject> ModelObjectHelper::CreateShareable() {
	return CreateInitialShareableFromPtr(new T());
}

#endif //SENIORRESEARCH_MODELOBJECTHELPER_H
