//
// Created by Tobiathan on 11/8/21.
//

#ifndef SENIORRESEARCH_MESHUTIL_H
#define SENIORRESEARCH_MESHUTIL_H

#include <tuple>
#include <vector>
#include "../vendor/glm/vec3.hpp"

class MeshUtil {
public:
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Quad3D(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4);
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Square(glm::vec3 midPoint, glm::vec3 dir, float sideLen);
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> Empty();
    static std::tuple<std::vector<glm::vec3>, std::vector<unsigned int>> PolyLine(const std::vector<glm::vec3>& points, float rad = 0.01f);
};


#endif //SENIORRESEARCH_MESHUTIL_H
