//
// Created by Tobiathan on 11/3/21.
//

#ifndef SENIORRESEARCH_LINELERPER_H
#define SENIORRESEARCH_LINELERPER_H


#include <vector>
#include "../vendor/glm/vec2.hpp"

class LineLerper {
public:
    static std::vector<glm::vec2> Lerp(const std::vector<glm::vec2>& vec1, const std::vector<glm::vec2>& vec2, float t);
    static std::vector<glm::vec2> MorphPolyLine(const std::vector<glm::vec2>& line1, const std::vector<glm::vec2>& line2, float t, int sampleCount);
};


#endif //SENIORRESEARCH_LINELERPER_H
