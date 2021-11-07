//
// Created by Tobiathan on 11/3/21.
//

#include "LineAnalyzer.h"
#include "../vendor/glm/geometric.hpp"

float LineAnalyzer::FullLength(std::vector<glm::vec2> linePoints) {

    float length = 0.0f;

    for (int i = 0; i < linePoints.size() - 1; i++) {
        length += glm::length(linePoints[i + 1] - linePoints[i]);
    }

    return length;
}
