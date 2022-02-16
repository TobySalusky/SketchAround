//
// Created by Tobiathan on 2/16/22.
//

#ifndef SENIORRESEARCH_EASEBLENDMODES_H
#define SENIORRESEARCH_EASEBLENDMODES_H


#include "BlendMode.h"

class ElasticInBlendMode : public BlendMode {
    [[nodiscard]] Enums::BlendType GetBlendType() const override { return Enums::ELASTIC_IN; }
    [[nodiscard]] std::string GetName() const override { return "Elastic-in"; }
};

class ElasticOutBlendMode : public BlendMode {
    [[nodiscard]] Enums::BlendType GetBlendType() const override { return Enums::ELASTIC_OUT; }
    [[nodiscard]] std::string GetName() const override { return "Elastic-out"; }
};

class ElasticInOutBlendMode : public BlendMode {
    [[nodiscard]] Enums::BlendType GetBlendType() const override { return Enums::ELASTIC_INOUT; }
    [[nodiscard]] std::string GetName() const override { return "Elastic-in/out"; }
};


#endif //SENIORRESEARCH_EASEBLENDMODES_H
