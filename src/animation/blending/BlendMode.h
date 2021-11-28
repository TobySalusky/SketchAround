//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_BLENDMODE_H
#define SENIORRESEARCH_BLENDMODE_H

#include "../../Enums.h"

class BlendMode {
public:
    [[nodiscard]] virtual Enums::BlendType GetBlendType() const = 0;

    [[nodiscard]] virtual int GetCustomID() const { return -1; }
    [[nodiscard]] virtual float ApplyCustomFunc(float t) const { return -1.0f; }

    static float Apply(BlendMode* mode1, BlendMode* mode2, float t);
};


#endif //SENIORRESEARCH_BLENDMODE_H
