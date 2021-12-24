//
// Created by Tobiathan on 12/21/21.
//

#include "Serialization.h"
#include <unordered_map>


Serialization::Serialization(const std::vector<ModelObject *>& modelObjects) {
    for (ModelObject* obj : modelObjects) {
        if (obj->GetType() == Enums::LATHE) {
            lathes.emplace_back((Lathe*) obj);
        } else if (obj->GetType() == Enums::CROSS_SECTIONAL) {
            crossSectionals.emplace_back((CrossSectional*) obj);
        } else {
            printf("ERROR: handle type please");
        }
    }
    order = Linq::Select<ModelObject*, int>(modelObjects, [&](ModelObject* obj) {
        obj->PrepSerialization();
        return obj->GetID();
    });
    blendModeManager = BlendModes::GetManager();
    blendModeManager.PrepSerialize();

    nextModelObjectUniqueID = ModelObject::GetCurrentNextUniqueID();
}

std::vector<ModelObject *> Serialization::Deserialize() {
    auto table = std::unordered_map<int, ModelObject*>();

    for (auto* el : lathes) { table[el->GetID()] = el; }
    for (auto* el : crossSectionals) { table[el->GetID()] = el; }

    auto res = Linq::Select<int, ModelObject*>(order, [&](int ID) {
        return table[ID];
    });

    for (auto* obj : res) {obj->UnParentRaw();}
    for (auto* obj : res) {obj->ReSerialize(table);}

    BlendModes::SetManager(blendModeManager);
    BlendModes::GetManager().ReSerialize();

    ModelObject::SetNextUniqueID(nextModelObjectUniqueID);

    return res;
}
