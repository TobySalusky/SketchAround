//
// Created by Tobiathan on 2/16/22.
//

#ifndef SENIORRESEARCH_EASINGFUNCTIONS_H
#define SENIORRESEARCH_EASINGFUNCTIONS_H


#include <charconv>

// SOURCE: https://easings.net/ && https://github.com/nicolausYes/easing-functions/blob/master/src/easing.cpp

class EasingFunctions {
public:
    static float EaseInElastic(float t) {
        const float c4 = (2 * PI) / 3.0f;

        return t == 0.0f ? 0.0f
            : t == 1.0f ? 1.0f
            : -pow(2.0f, 10.0f * t - 10.0f) * sin((t * 10.0f - 10.75f) * c4);

    }

    static float EaseOutElastic(float t) {
        float t2 = (t - 1) * (t - 1);
        return 1 - t2 * t2 * cos( t * PI * 4.5f );
    }

    static float EaseInOutElastic(float t) {
        float t2;
        if (t < 0.45f) {
            t2 = t * t;
            return 8.0f * t2 * t2 * sin( t * PI * 9 );
        } else if (t < 0.55f) {
            return 0.5f + 0.75f * sin( t * PI * 4 );
        } else {
            t2 = (t - 1) * (t - 1);
            return 1 - 8 * t2 * t2 * sin( t * PI * 9 );
        }
    }

private:
    static constexpr float PI = (float) M_PI;
};


#endif //SENIORRESEARCH_EASINGFUNCTIONS_H
