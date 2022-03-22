//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_REVOLVER_H
#define SENIORRESEARCH_REVOLVER_H


#include "../vendor/glm/vec2.hpp"
#include "../util/Includes.h"
#include "TopologyCorrector.h"

struct RevolveData {
    float scaleRadius{};
    float scaleZ{};
    float scaleY{};
    int countPerRing{};
    float leanScalar{};
    bool wrapStart{};
    bool wrapEnd{};
    const Vec2List& graphY;
    const Vec2List& graphZ;
    const Vec2List& crossSectionPoints;
    std::optional<std::function<Vec2List(float)>>
    crossSectionSnapPointFunc;
};

class Revolver {
public:
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>>
    Revolve(const std::vector<glm::vec2> &points,
			const RevolveData& revolveData, TopologyCorrector* outTopologyData = nullptr);
};


#endif //SENIORRESEARCH_REVOLVER_H
