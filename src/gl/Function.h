//
// Created by Tobiathan on 10/15/21.
//

#ifndef SENIORRESEARCH_FUNCTION_H
#define SENIORRESEARCH_FUNCTION_H

#include "../vendor/glm/glm.hpp"
#include <vector>

class Function {
public:
    static float GetY(std::vector<glm::vec2> funcPoints, float x);
};


#endif //SENIORRESEARCH_FUNCTION_H
