//
// Created by Tobiathan on 11/28/21.
//

#ifndef SENIORRESEARCH_PIECEWISEBLENDMODE_H
#define SENIORRESEARCH_PIECEWISEBLENDMODE_H


#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include "BlendMode.h"
#include "../../util/Includes.h"

class PiecewiseBlendMode : public BlendMode {
public:
    explicit PiecewiseBlendMode(const Vec2List& funcPoints, int ID);

    [[nodiscard]] Enums::BlendType GetBlendType() const override;

    [[nodiscard]] int GetCustomID() const override;

    [[nodiscard]] float ApplyCustomFunc(float t) const override;
    [[nodiscard]] std::string GetName() const override;

    PiecewiseBlendMode() {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & ID;
        ar & funcPoints;
    }

    Vec2List funcPoints;
    int ID;
};


#endif //SENIORRESEARCH_PIECEWISEBLENDMODE_H
