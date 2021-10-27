//
// Created by Tobiathan on 10/26/21.
//

#ifndef SENIORRESEARCH_LATHE_H
#define SENIORRESEARCH_LATHE_H

#include <vector>
#include "../vendor/glm/vec2.hpp"
#include "../vendor/glm/vec4.hpp"
#include "../vendor/glm/vec3.hpp"
#include "../gl/Mesh.h"


class Lathe {
public:
    Lathe() = default;

    std::vector<glm::vec2> plottedPoints;
    std::vector<glm::vec2> graphedPointsY;
    std::vector<glm::vec2> graphedPointsZ;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
private:
};


#endif //SENIORRESEARCH_LATHE_H
