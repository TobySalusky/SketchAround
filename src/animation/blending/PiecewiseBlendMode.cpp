//
// Created by Tobiathan on 11/28/21.
//

#include "PiecewiseBlendMode.h"
#include "../../graphing/Function.h"
#include "../../util/Linq.h"

Enums::BlendType PiecewiseBlendMode::GetBlendType() const {
    return Enums::CUSTOM;
}

int PiecewiseBlendMode::GetCustomID() const {
    return ID;
}

float PiecewiseBlendMode::ApplyCustomFunc(float t) const {
    return Function::GetY(funcPoints, t);
}

PiecewiseBlendMode::PiecewiseBlendMode(const Vec2List& funcPoints, int ID) {
    Vec2 initPoint = funcPoints.front();
    Vec2 lastPoint = funcPoints.back();
    Vec2 diff = lastPoint - initPoint;

    this->funcPoints = Linq::Select<Vec2, Vec2>(funcPoints, [=](Vec2 vec) {
        return (vec - initPoint) / diff;
    });

    this->ID = ID;
}

std::string PiecewiseBlendMode::GetName() const {
    return "Custom";
}
