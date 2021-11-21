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
    Enums::KeyframeBlendMode blendMode = Enums::LINEAR;

    static T GetAnimatedValueAtT(const KeyFrame<T>& frame1, const KeyFrame<T>& frame2, float t) {
        if (t <= 0.0f) return frame1.val;
        if (t >= 1.0f) return frame2.val;

        return Lerp(frame1, frame2, t);
    }

    static float Lerp(const KeyFrame<float>& frame1, const KeyFrame<float>& frame2, float t) {
        return frame1.val + (frame2.val - frame1.val) * t;
    }

    static float RemapTime(float t, const KeyFrame<T>& frame1, const KeyFrame<T>& frame2) { // TODO: allow t outside {0, 1} for recoil
        switch (frame1.blendMode) {
            case Enums::LINEAR:
                switch (frame2.blendMode) {
                    case Enums::LINEAR:
                        return t;
                        break;
                    case Enums::SINE:
                        //return t * (-cos(3 * (float) M_PI * t) + 1.0f) / 2.0f; TODO: bouncy type effect
                        return sin((float) M_PI_2 * t);
                        break;
                }
                break;
            case Enums::SINE:
                switch (frame2.blendMode) {
                    case Enums::LINEAR:
                        return sin((float) M_PI_2 * t - (float) M_PI_2) + 1.0f;
                        break;
                    case Enums::SINE:
                        return Util::SinSmooth01(t);
                        break;
                }
                break;
        }
    }


    static std::vector<glm::vec2> Lerp(const KeyFrame<std::vector<glm::vec2>>& frame1, const KeyFrame<std::vector<glm::vec2>>& frame2, float t) {
        // TODO: better count??
        return LineLerper::MorphPolyLine(frame1.val, frame2.val, t, (int) std::max(frame1.val.size(), frame2.val.size()));
    }
};

#endif //SENIORRESEARCH_KEYFRAME_H
