//
// Created by Tobiathan on 11/28/21.
//

#include <cmath>
#include "BlendMode.h"
#include "../../util/Util.h"
#include "../../util/EasingFunctions.h"

// TODO: extract functionality to subclasses
float BlendMode::Apply(BlendMode *mode1, BlendMode *mode2, float t) {
    const auto type1 = mode1->GetBlendType();
    const auto type2 = mode2->GetBlendType();

    if (type1 == type2) { // TODO: REFACTOR!!!
        if (type1 == Enums::ELASTIC_IN) return EasingFunctions::EaseInElastic(t);
        if (type1 == Enums::ELASTIC_OUT) return EasingFunctions::EaseOutElastic(t);
        if (type1 == Enums::ELASTIC_INOUT) return EasingFunctions::EaseInOutElastic(t);
    }

    switch (type1) {
        case Enums::LINEAR:
            switch (type2) {
                case Enums::LINEAR:
                    return t;
                    break;
                case Enums::SINE:
                    // return t * (-cos(3 * (float) M_PI * t) + 1.0f) / 2.0f; TODO: bouncy type effect
                    // return \frac{-\cos\left(1.38\pi x-\pi\cdot0.19\right)\cdot1.2+1}{2}
                    // return (-cos(1.38 * (float) M_PI * t - (float) M_PI * 0.19) * 1.2f + 1.0f) / 2.0f;
                    return sin((float) M_PI_2 * t);
                    break;
                case Enums::CUSTOM:
                    return t;
                    break;
            }
            break;
        case Enums::SINE:
            switch (type2) {
                case Enums::LINEAR:
                    return sin((float) M_PI_2 * t - (float) M_PI_2) + 1.0f;
                    break;
                case Enums::SINE:
                    return Util::SinSmooth01(t);
                    break;
                case Enums::CUSTOM:
                    return t;
                    break;
            }
            break;
        case Enums::CUSTOM:
            if (type2 != Enums::CUSTOM || mode1->GetCustomID() != mode2->GetCustomID()) return t;
            return mode1->ApplyCustomFunc(t);
            break;
    }

    return t;
}
