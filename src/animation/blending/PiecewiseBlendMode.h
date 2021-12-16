//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_PIECEWISEBLENDMODE_H
#define SENIORRESEARCH_PIECEWISEBLENDMODE_H


#include "BlendMode.h"
#include "../../util/Includes.h"

class PiecewiseBlendMode : public BlendMode {
public:
    explicit PiecewiseBlendMode(const Vec2List& funcPoints, int ID);

    [[nodiscard]] Enums::BlendType GetBlendType() const override;

    [[nodiscard]] int GetCustomID() const override;

    [[nodiscard]] float ApplyCustomFunc(float t) const override;
    [[nodiscard]] std::string GetName() const override;

private:
    Vec2List funcPoints;
    int ID;
};


#endif //SENIORRESEARCH_PIECEWISEBLENDMODE_H
