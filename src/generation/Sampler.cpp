//
// Created by Tobiathan on 9/21/21.
//

#include "Sampler.h"
#include "../vendor/glm/geometric.hpp"

std::vector<glm::vec2> Sampler::DumbSample(std::vector<glm::vec2> inputPoints, const float diff) {
    std::vector<glm::vec2> sampled{inputPoints[0]};

    float currDiff = diff;
    for (size_t i = 1; i < inputPoints.size(); i++) {
        glm::vec p1 = inputPoints[i - 1];
        glm::vec vec = inputPoints[i] - p1;
        float mag = glm::length(vec);
        float magUsed = 0;
        while (currDiff <= mag) {
            sampled.emplace_back(p1 + glm::normalize(vec) * (currDiff + magUsed));
            mag -= currDiff;
            currDiff = diff;
        }
        currDiff -= mag;
    }

    sampled.emplace_back(inputPoints[inputPoints.size() - 1]);
    return sampled;
}
