//
// Created by Tobiathan on 9/21/21.
//

#include "Sampler.h"
#include "../vendor/glm/geometric.hpp"
#include "LineAnalyzer.h"
#include "../util/Util.h"

// FIXME: bug here! doesn't sample linear segments!
std::vector<glm::vec2> Sampler::DumbSample(const std::vector<glm::vec2>& inputPoints, const float diff) {
    std::vector<glm::vec2> sampled{inputPoints[0]};

    float currDiff = 0.0f;
    for (size_t i = 1; i < inputPoints.size(); i++) {
        const Vec2 p1 = inputPoints[i - 1];
        const Vec2 vec = inputPoints[i] - p1;
        const Vec2 norm = glm::normalize(vec);
        const float mag = glm::length(vec);
        const float startingDiff = currDiff;
        currDiff += mag;

        const int iter = (int) (currDiff / diff);
        for (int j = 0; j < iter; j++) {
            sampled.emplace_back(p1 + norm * ((diff * (float) (j + 1)) - startingDiff));
            currDiff -= diff;
        }
    }

    sampled.emplace_back(inputPoints[inputPoints.size() - 1]);
    return sampled;
}

// FIXME: bug here! doesn't sample linear segments!
std::vector<glm::vec2> Sampler::DumbSampleLimited(const std::vector<glm::vec2>& inputPoints, const float diff, const int limit) {

    std::vector<glm::vec2> sampled{inputPoints[0]};

    if (sampled.size() < limit - 1) {
        float currDiff = 0.0f;
        for (size_t i = 1; i < inputPoints.size(); i++) {
            const Vec2 p1 = inputPoints[i - 1];
            const Vec2 vec = inputPoints[i] - p1;
            const Vec2 norm = glm::normalize(vec);
            const float mag = glm::length(vec);
            const float startingDiff = currDiff;
            currDiff += mag;

            const int iter = (int) (currDiff / diff);
            for (int j = 0; j < iter; j++) {
                sampled.emplace_back(p1 + norm * ((diff * (float) (j + 1)) - startingDiff));
                currDiff -= diff;
            }
            if (sampled.size() == limit - 1) break;
        }
    }

    if (sampled.size() == limit - 1) sampled.emplace_back(inputPoints[inputPoints.size() - 1]);
    return sampled;
}

Vec2List
Sampler::SampleTo(const Vec2List& inputPoints, const int newPointCount, bool skipIfAlreadyCorrect) { // FIXME: DOES NOT WORK CORRECTLY!!! ALSO INEFFICIENT!!
    if (skipIfAlreadyCorrect && newPointCount == inputPoints.size()) return inputPoints;

    Vec2List vec = (inputPoints.size() == 1) ? RepeatPoint(inputPoints[0], newPointCount) : DumbSampleLimited(inputPoints, LineAnalyzer::FullLength(inputPoints) / (float) (newPointCount - 1), newPointCount);
    if (vec.size() != newPointCount) LOG("Error: SampleTo %i %lu", newPointCount, vec.size());
    return vec;
}

Vec2List Sampler::RepeatPoint(Vec2 point, const int count) {
    Vec2List vec = {};
    vec.reserve(count);
    for (int i = 0; i < count; i++) {
        vec.emplace_back(point);
    }
    return vec;
}
