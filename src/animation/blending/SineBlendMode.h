//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_SINEBLENDMODE_H
#define SENIORRESEARCH_SINEBLENDMODE_H


#include "BlendMode.h"

class SineBlendMode : public BlendMode {
public:
    [[nodiscard]] Enums::BlendType GetBlendType() const override;
};


#endif //SENIORRESEARCH_SINEBLENDMODE_H
