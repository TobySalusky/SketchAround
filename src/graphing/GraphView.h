//
// Created by Tobiathan on 10/19/21.
//

#ifndef SENIORRESEARCH_GRAPHVIEW_H
#define SENIORRESEARCH_GRAPHVIEW_H


#include "../vendor/glm/ext/matrix_float4x4.hpp"

struct GraphView {
    float minX, maxX, minY, maxY;

    GraphView(float minX, float maxX, float minY, float maxY);

    glm::mat4 GenProjection() const;
};


#endif //SENIORRESEARCH_GRAPHVIEW_H
