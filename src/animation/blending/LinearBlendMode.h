//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_LINEARBLENDMODE_H
#define SENIORRESEARCH_LINEARBLENDMODE_H


#include "BlendMode.h"

class LinearBlendMode : public BlendMode {
public:
    [[nodiscard]] Enums::BlendType GetBlendType() const override;
};


#endif //SENIORRESEARCH_LINEARBLENDMODE_H
