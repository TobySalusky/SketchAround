//
// Created by Tobiathan on 11/29/21.
//

#ifndef SENIORRESEARCH_ANIMATOR_H
#define SENIORRESEARCH_ANIMATOR_H

#include <unordered_map>
#include "../util/Includes.h"
#include "KeyFrameLayer.h"

class Animator {
public:
    struct SettableFloatKeyFrameLayer {
        KeyFrameLayer<float> layer;
        float* floatPtr{};
    };

    Animator();



private:
    float currentTime = 0.0f;
    std::unordered_map<Enums::DrawMode, KeyFrameLayer<std::vector<glm::vec2>>> keyFrameLayers;
    std::unordered_map<std::string, SettableFloatKeyFrameLayer> floatKeyFrameLayers;

    friend class Timeline;
};


#endif //SENIORRESEARCH_ANIMATOR_H
