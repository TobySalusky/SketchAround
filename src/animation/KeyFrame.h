//
// Created by Tobiathan on 11/14/21.
//

#ifndef SENIORRESEARCH_KEYFRAME_H
#define SENIORRESEARCH_KEYFRAME_H

#include "../vendor/glm/vec2.hpp"
#include "LineLerper.h"
#include "blending/BlendMode.h"
#include "blending/LinearBlendMode.h"
#include "blending/BlendModeManager.h"
#include "blending/BlendModes.h"
#include <vector>

template <typename T>
struct KeyFrame {
    T val {};
    float time {};
    int blendModeID = 0;

    static T GetAnimatedValueAtT(const KeyFrame<T>& frame1, const KeyFrame<T>& frame2, float t) {
        return Lerp(frame1, frame2, t);
    }

    static float Lerp(const KeyFrame<float>& frame1, const KeyFrame<float>& frame2, float t) {
        return frame1.val + (frame2.val - frame1.val) * t;
    }

    static float RemapTime(float t, const KeyFrame<T>& frame1, const KeyFrame<T>& frame2) { // TODO: allow t outside {0, 1} for recoil
        return BlendMode::Apply(BlendModes::Get(frame1.blendModeID), BlendModes::Get(frame2.blendModeID), t);
    }


    static std::vector<glm::vec2> Lerp(const KeyFrame<std::vector<glm::vec2>>& frame1, const KeyFrame<std::vector<glm::vec2>>& frame2, float t) {
        // TODO: better count??
        return LineLerper::MorphPolyLine(frame1.val, frame2.val, t, (int) std::max(frame1.val.size(), frame2.val.size()));
    }
};

#endif //SENIORRESEARCH_KEYFRAME_H
