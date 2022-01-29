//
// Created by Tobiathan on 10/15/21.
//

#include "Function.h"

float Function::GetY(const std::vector<glm::vec2>& funcPoints, float x) {

    auto upper = std::upper_bound(funcPoints.begin(), funcPoints.end(), x,
  [](float value, const glm::vec2& vec) {
            return value < vec.x;
        }
    );

    if (upper == funcPoints.begin() || upper == funcPoints.end()) return 0;

    glm::vec2 p1 = upper[-1];
    glm::vec2 p2 = upper[0];

    return std::lerp(p1.y, p2.y, (x - p1.x) / (p2.x - p1.x));
}

Vec2 Function::GetTangent(const std::vector<glm::vec2>& funcPoints, float x) {
    auto upper = std::upper_bound(funcPoints.begin(), funcPoints.end(), x,
                                  [](float value, const glm::vec2& vec) {
                                      return value < vec.x;
                                  }
    );

    if (upper == funcPoints.begin() || upper == funcPoints.end()) return {0.01f, 0.0f}; // TODO: FIXME:

    glm::vec2 p1 = upper[-1];
    glm::vec2 p2 = upper[0];

    return p2 - p1;
}

float Function::GetRadians(const std::vector<glm::vec2>& funcPoints, float x) {
    return Util::Angle(GetTangent(funcPoints, x));
}

float Function::GetAverageRadians(const std::vector<glm::vec2> &funcPoints, float x, int count) {
    auto upper = std::upper_bound(funcPoints.begin(), funcPoints.end(), x,
                                  [](float value, const glm::vec2& vec) {
                                      return value < vec.x;
                                  }
    );

    if (upper == funcPoints.begin() || upper == funcPoints.end()) return 0;

    Vec2 tangentSum = {0.0f, 0.0f};

    const auto addSlope = [&](glm::vec2 p1, glm::vec2 p2) {
        tangentSum += (p2 - p1);
    };

    addSlope(upper[-1], upper[0]);

    for (int i = -1; i >= -count; i--) {
        if (&upper[i - 1] == &funcPoints[0]) break;
        addSlope(upper[i - 1], upper[i]);
    }

    for (int i = 1; i <= count; i++) { // FIXME: sus point stuff
        if (&upper[i] == &funcPoints[funcPoints.size() - 1]) break;
        addSlope(upper[i - 1], upper[i]);
    }

    return Util::Angle(tangentSum);
}

Vec2 Function::GetTangentAvgAtIndex(const std::vector<glm::vec2> &funcPoints, int segmentIndex, int count) {
    Vec2 tangentSum = {0.0f, 0.0f};
    int realCount = 0;

    const auto addSlope = [&](glm::vec2 p1, glm::vec2 p2) {
        tangentSum += (p2 - p1);
        realCount++;
    };

    addSlope(funcPoints[segmentIndex], funcPoints[segmentIndex + 1]);

    for (int i = -1; i >= -count; i--) {
        if (segmentIndex + i < 0) break;
        addSlope(funcPoints[segmentIndex + i], funcPoints[i]);
    }

    for (int i = 1; i <= count; i++) { // FIXME: sus point stuff
        if (segmentIndex + i >= funcPoints.size()) break;
        addSlope(funcPoints[segmentIndex], funcPoints[segmentIndex + i]);
    }

    return tangentSum / (float) realCount;
}

Vec2List Function::GetAvgTangentsAlongLength(const std::vector<glm::vec2> &funcPoints, float lengthPer, int count) {

    Vec2List avgTangents;
    float runningLength = 0.0f;
    for (int i = 0; i < funcPoints.size() - 1; i++) {
        runningLength += glm::length(funcPoints[i + 1] - funcPoints[i]);


    }

    return avgTangents;
}
