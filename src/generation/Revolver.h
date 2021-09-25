//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_REVOLVER_H
#define SENIORRESEARCH_REVOLVER_H


#include "../vendor/glm/vec2.hpp"

class Revolver {
public:
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Revolve(const std::vector<glm::vec2> &points, int countPerRing = 3);
};


#endif //SENIORRESEARCH_REVOLVER_H
