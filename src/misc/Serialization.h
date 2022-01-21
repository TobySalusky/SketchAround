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

    explicit Serialization(const std::vector<ModelObject*>& modelObjects, const std::vector<unsigned char>& snapshotImg);

    std::vector<unsigned char> img = {};
    std::vector<int> order = {};
    std::vector<Lathe*> lathes = {};
    std::vector<CrossSectional*> crossSectionals = {};
    int nextModelObjectUniqueID{};
    BlendModeManager blendModeManager;

    static void SetReadMetaOnly(bool _readMetaOnly) { readMetaOnly = _readMetaOnly; }

private:
    static bool readMetaOnly;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        const unsigned int imgVersion = 1;
        if (version >= imgVersion) ar & img;

        if (!readMetaOnly) {
            ar & order;
            ar & lathes;
            ar & crossSectionals;
            ar & nextModelObjectUniqueID;
            ar & blendModeManager;
        }
    }
};
BOOST_CLASS_VERSION(Serialization, 1)

#endif //SENIORRESEARCH_SERIALIZATION_H
