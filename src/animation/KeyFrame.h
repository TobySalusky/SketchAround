//
// Created by Tobiathan on 11/14/21.
//

#ifndef SENIORRESEARCH_KEYFRAME_H
#define SENIORRESEARCH_KEYFRAME_H

#include "../vendor/glm/vec2.hpp"
#include "LineLerper.h"
#include <vector>

template <typename T>
struct KeyFrame {
    T val;
    float time;

    static T GetAnimatedValueAtT(const KeyFrame<T>& frame1, const KeyFrame<T>& frame2, float t) {
        if (t <= 0.0f) return frame1.val;
        if (t >= 1.0f) return frame2.val;

        return Lerp(frame1, frame2, t);
    }

    static std::vector<glm::vec2> Lerp(const KeyFrame<std::vector<glm::vec2>>& frame1, const KeyFrame<std::vector<glm::vec2>>& frame2, float t) {
        // TODO: find count
        return LineLerper::MorphPolyLine(frame1.val, frame2.val, t, (int) std::max(frame1.val.size(), frame2.val.size()));
    }
};

#endif //SENIORRESEARCH_KEYFRAME_H
