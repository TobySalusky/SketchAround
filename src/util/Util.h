//
// Created by Tobiathan on 9/19/21.
//

#ifndef SENIORRESEARCH_UTIL_H
#define SENIORRESEARCH_UTIL_H


#include <iostream>
#include "../vendor/glm/glm.hpp"

class Util {
public:
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

    static float sin01(float x) {
        return (sin(x) + 1.0f) / 2.0f;
    }
};


#endif //SENIORRESEARCH_UTIL_H
