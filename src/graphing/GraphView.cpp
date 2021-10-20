//
// Created by Tobiathan on 10/19/21.
//

#include "GraphView.h"
#include "../vendor/glm/ext/matrix_transform.hpp"

glm::mat4 GraphView::GenProjection() const {

    glm::mat4 mat {1.0f};

    /*mat = glm::translate(glm::mat4(1.0f), {-minX, 0.0f, 0.0f})
            * glm::scale(glm::mat4(1.0f), {maxX - minX, 1.0f, 1.0f})
            * glm::scale(glm::mat4(1.0f), {2.0f, 1.0f, 1.0f})
            * glm::translate(glm::mat4(1.0f), {-1.0f, 0.0f, 0.0f});*/

    // 1.0f / ((maxX - minX) / 2.0f)

    //mat = glm::scale(glm::translate(mat, {0.5f, 0.0f, 0.0f}), {0.5f, 1.0f, 1.0f});
    //mat = glm::scale(mat, {2.0f, 1.0f, 1.0f});

    return mat;
}

GraphView::GraphView(float minX, float maxX, float minY, float maxY) : minX(minX), maxX(maxX), minY(minY), maxY(maxY) {

}
