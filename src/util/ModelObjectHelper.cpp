//
// Created by Tobiathan on 4/15/22.
//

#include "ModelObjectHelper.h"


std::shared_ptr<ModelObject> ModelObjectHelper::CreateInitialShareableFromPtr(ModelObject *objPtr) {
	auto shared = std::shared_ptr<ModelObject>(objPtr);
	std::weak_ptr<ModelObject> weak = shared;

	shared->InitializeWithWeakPtr(weak);

	return shared;
}
