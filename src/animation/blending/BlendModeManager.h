//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_BLENDMODEMANAGER_H
#define SENIORRESEARCH_BLENDMODEMANAGER_H

#include <unordered_map>
#include "BlendMode.h"
#include "PiecewiseBlendMode.h"
#include "SineBlendMode.h"
#include "LinearBlendMode.h"
#include "EaseBlendModes.h"
#include <vector>
#include <boost/serialization/access.hpp>
#include <boost/serialization/unordered_map.hpp>

// FIXME: NEVER CLEANED UP (perhaps use unique_ptr?)
class BlendModeManager {
public:
    BlendModeManager() = default;

    BlendMode* Get(int ID) { return blendModes[ID]; }

    void Add(BlendMode* newBlendMode) { blendModes[GenNextID()] = newBlendMode; }

    std::vector<int> GenAllIDs();

    [[nodiscard]] int GetNextID() const { return nextID; }

    void PrepSerialize() {
        serializeCustomBlendModes = {};
        for (auto& [ID, blendMode] : blendModes) {
            if (blendMode->GetBlendType() == Enums::CUSTOM) serializeCustomBlendModes[ID] = (PiecewiseBlendMode*) blendMode;
        }
    }

    void ReSerialize() {
        blendModes = {};
        blendModes[0] = new LinearBlendMode();
        blendModes[1] = new SineBlendMode();
        blendModes[2] = new ElasticInBlendMode();
        blendModes[3] = new ElasticOutBlendMode();
        blendModes[4] = new ElasticInOutBlendMode();
        nextID = 2;
        for (auto& [ID, blendMode] : serializeCustomBlendModes) {
            blendModes[ID] = blendMode;
            nextID = ID + 1;
        }
        serializeCustomBlendModes = {};
    }

    void Init() {
        blendModes[GenNextID()] = new LinearBlendMode();
        blendModes[GenNextID()] = new SineBlendMode();
        blendModes[GenNextID()] = new ElasticInBlendMode();
        blendModes[GenNextID()] = new ElasticOutBlendMode();
        blendModes[GenNextID()] = new ElasticInOutBlendMode();

    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & nextID;
        ar & serializeCustomBlendModes;
    }

    int GenNextID() { return nextID++; }

    int nextID {};
    std::unordered_map<int, BlendMode*> blendModes;
    std::unordered_map<int, PiecewiseBlendMode*> serializeCustomBlendModes;
};


#endif //SENIORRESEARCH_BLENDMODEMANAGER_H
