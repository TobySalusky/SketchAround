//
// Created by Tobiathan on 9/21/21.
//

#include "Sampler.h"
#include "../vendor/glm/geometric.hpp"
#include "LineAnalyzer.h"

// FIXME: bug here! doesn't sample linear segments!
std::vector<glm::vec2> Sampler::DumbSample(const std::vector<glm::vec2>& inputPoints, const float diff) {
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

// FIXME: bug here! doesn't sample linear segments!
std::vector<glm::vec2> Sampler::DumbSampleLimited(const std::vector<glm::vec2>& inputPoints, const float diff, const int limit) {
    std::vector<glm::vec2> sampled{inputPoints[0]};

    float currDiff = diff;

    if (sampled.size() < limit - 1) {
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
            if (sampled.size() == limit - 1) break;
            currDiff -= mag;
        }
    }

    sampled.emplace_back(inputPoints[inputPoints.size() - 1]);
    return sampled;
}

std::vector<glm::vec2>
Sampler::SampleTo(const std::vector<glm::vec2>& inputPoints, const int newPointCount) { // FIXME: DOES NOT WORK CORRECTLY!!! ALSO INEFFICIENT!!
    std::vector<glm::vec2> vec = DumbSampleLimited(inputPoints, LineAnalyzer::FullLength(inputPoints) / (float) (newPointCount - 1), newPointCount);
    //printf("%i, %lu\n", newPointCount, vec.size());
    return vec;
}
