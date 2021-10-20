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

float Function::GetSlope(const std::vector<glm::vec2>& funcPoints, float x) {
    auto upper = std::upper_bound(funcPoints.begin(), funcPoints.end(), x,
                                  [](float value, const glm::vec2& vec) {
                                      return value < vec.x;
                                  }
    );

    if (upper == funcPoints.begin() || upper == funcPoints.end()) return 0;

    glm::vec2 p1 = upper[-1];
    glm::vec2 p2 = upper[0];

    return (p2.y - p1.y) / (p2.x - p1.x);
}

float Function::GetSlopeRadians(const std::vector<glm::vec2>& funcPoints, float x) {
    return std::atan2(GetSlope(funcPoints, x), 1.0f);
}
