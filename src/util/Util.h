//
// Created by Tobiathan on 9/19/21.
//

#ifndef SENIORRESEARCH_UTIL_H
#define SENIORRESEARCH_UTIL_H


#include <iostream>
#include <vector>
#include <numeric>
#include "../vendor/glm/glm.hpp"
#include "Rectangle.h"

typedef glm::vec2 Vec2;
typedef glm::vec2 Vec3;
typedef glm::vec4 Vec4;
typedef glm::vec4 RGBA;
typedef std::vector<glm::vec2> Vec2List;
typedef std::vector<glm::vec3> Vec3List;



class Util {
public:
    static glm::vec2 AveragePos(const std::vector<glm::vec2>& points);

    static void LogVec3(glm::vec3 vec) {
        std::cout << '<' << vec.x << ", " << vec.y << ", " << vec.z << ">\n";
    }
    static std::string ReadFile(const char* path);

    static glm::vec2 Polar(float mag, float angle);
    static glm::vec2 Polar(float angle) {
        return Polar(1.0f, angle);
    }

    static float Angle(glm::vec2 vec) {
        return atan2(vec.y, vec.x);
    }

    static float SlopeToRadians(float slope) {
        return std::atan2(slope, 1.0f);
    }

    static float Lerp(float f1, float f2, float t) {
        return f1 * (1 - t) + f2 * t;
    }

    static float Sin01(float x) {
        return (sin(x) + 1.0f) / 2.0f;
    }

    static glm::vec2 NormalizeToRect01(glm::vec2 pos, Rectangle rect) {
        return {(pos.x - rect.x) / rect.width, ((pos.y - rect.y) / rect.height)};
    }

    static glm::vec2 NormalizeToRect01Flipped(glm::vec2 pos, Rectangle rect) {
        return NormalizeToRect01(pos, rect) * glm::vec2(1.0f, -1.0f);
    }

    static glm::vec2 NormalizeToRectNP(glm::vec2 pos, Rectangle rect) {
        return NormalizeToRect01(pos, rect) * 2.0f - glm::vec2(1.0f, 1.0f);
    }

    static glm::vec2 NormalizeToRectNPFlipped(glm::vec2 pos, Rectangle rect) {
        return NormalizeToRectNP(pos, rect) * glm::vec2(1.0f, -1.0f);
    }

    static bool VecIsNormalizedNP(glm::vec2 vec) {
        return vec.x >= -1.0f && vec.x <= 1.0f && vec.y >= -1.0f && vec.y <= 1.0f;
    }

    static float SinSmooth01(float t) {
        if (t < 0.0f) return 0.0f;
        if (t > 1.0f) return 1.0f;
        return (sin((t * (float) M_PI) - ((float) M_PI / 2.0f)) + 1.0f) / 2.0f;
    }
};


#endif //SENIORRESEARCH_UTIL_H
