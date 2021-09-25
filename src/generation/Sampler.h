//
// Created by Tobiathan on 9/21/21.
//

#ifndef SENIORRESEARCH_SAMPLER_H
#define SENIORRESEARCH_SAMPLER_H


#include <vector>
#include "../vendor/glm/vec2.hpp"

class Sampler {
public:
    static std::vector<glm::vec2> DumbSample(std::vector<glm::vec2> inputPoints, const float diff = 0.025f);
};


#endif //SENIORRESEARCH_SAMPLER_H
