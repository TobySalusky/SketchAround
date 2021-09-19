//
// Created by Tobiathan on 9/19/21.
//

#ifndef SENIORRESEARCH_UTIL_H
#define SENIORRESEARCH_UTIL_H


#include <iostream>
#include "vendor/glm/glm.hpp"

class Util {
public:
    static void LogVec3(glm::vec3 vec) {
        std::cout << '<' << vec.x << ", " << vec.y << ", " << vec.z << ">\n";
    }
};


#endif //SENIORRESEARCH_UTIL_H
