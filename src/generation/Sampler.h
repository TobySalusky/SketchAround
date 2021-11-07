//
// Created by Tobiathan on 9/21/21.
//

#ifndef SENIORRESEARCH_SAMPLER_H
#define SENIORRESEARCH_SAMPLER_H


#include <vector>
#include "../vendor/glm/vec2.hpp"

class Sampler {
public:
    static std::vector<glm::vec2> DumbSample(const std::vector<glm::vec2>& inputPoints, float diff = 0.025f);
    static std::vector<glm::vec2> SampleTo(const std::vector<glm::vec2>& inputPoints, int newPointCount);
private:
    static std::vector<glm::vec2> DumbSampleLimited(const std::vector<glm::vec2>& inputPoints, float diff, int limit);
};


#endif //SENIORRESEARCH_SAMPLER_H
