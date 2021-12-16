//
// Created by Tobiathan on 11/28/21.
//

#include "SineBlendMode.h"

Enums::BlendType SineBlendMode::GetBlendType() const {
    return Enums::SINE;
}

std::string SineBlendMode::GetName() const {
    return "Sine";
}
