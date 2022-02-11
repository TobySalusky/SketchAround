//
// Created by Tobiathan on 10/15/21.
//

#ifndef SENIORRESEARCH_FUNCTION_H
#define SENIORRESEARCH_FUNCTION_H

#include "../vendor/glm/glm.hpp"
#include "../util/Util.h"
#include "../generation/Intersector.h"
#include <vector>

class Function {
public:
    static float GetY(const std::vector<glm::vec2>& funcPoints, float x);
    static Vec2 GetTangent(const std::vector<glm::vec2>& funcPoints, float x);
    static Vec2 GetTangentAtLength(const std::vector<glm::vec2>& funcPoints, float arcLength);
    static Ray2D GetRayAtLength(const std::vector<glm::vec2>& funcPoints, float arcLength);
    static Vec2List GetAvgTangentsAlongLength(const std::vector<glm::vec2>& funcPoints, float lengthPer, int count);
    static float GetAverageRadians(const std::vector<glm::vec2>& funcPoints, float x, int count);
    static float GetRadians(const std::vector<glm::vec2>& funcPoints, float x);

private:
    static Vec2 GetTangentAvgAtIndex(const std::vector<glm::vec2> &funcPoints, int segmentIndex, int count);
};


#endif //SENIORRESEARCH_FUNCTION_H
