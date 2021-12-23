//
// Created by Tobiathan on 12/21/21.
//

#ifndef SENIORRESEARCH_SERIALIZATION_H
#define SENIORRESEARCH_SERIALIZATION_H


#include <vector>
#include "../generation/ModelObject.h"
#include "../generation/Lathe.h"
#include "../generation/CrossSectional.h"

class Serialization {
public:
    std::vector<ModelObject*> Deserialize();

    Serialization() = default;

    explicit Serialization(const std::vector<ModelObject*>& modelObjects);

    std::vector<int> order = {};
    std::vector<Lathe*> lathes = {};
    std::vector<CrossSectional*> crossSectionals = {};
    BlendModeManager blendModeManager;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & order;
        ar & lathes;
        ar & crossSectionals;
        ar & blendModeManager;
    }
};


#endif //SENIORRESEARCH_SERIALIZATION_H
