//
// Created by Tobiathan on 11/3/21.
//

#ifndef SENIORRESEARCH_LINEANALYZER_H
#define SENIORRESEARCH_LINEANALYZER_H

#include <vector>
#include "../vendor/glm/vec2.hpp"

class LineAnalyzer {
public:
    static float FullLength(std::vector<glm::vec2> linePoints);
};


#endif //SENIORRESEARCH_LINEANALYZER_H
