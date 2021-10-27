//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_REVOLVER_H
#define SENIORRESEARCH_REVOLVER_H


#include "../vendor/glm/vec2.hpp"

struct RevolveData {
    float scaleRadius{};
    float scaleZ{};
    float scaleY{};
    int countPerRing{};
    float leanScalar{};
    std::vector<glm::vec2> *graphYPtr{};
    std::vector<glm::vec2> *graphZPtr{};
};

class Revolver {
public:
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
    Revolve(const std::vector<glm::vec2> &points, const RevolveData& revolveData);
};


#endif //SENIORRESEARCH_REVOLVER_H
