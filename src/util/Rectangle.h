//
// Created by Tobiathan on 11/13/21.
//

#ifndef SENIORRESEARCH_RECTANGLE_H
#define SENIORRESEARCH_RECTANGLE_H


#include "../vendor/glm/vec2.hpp"

struct Rectangle {
    float x{};
    float y{};
    float width{};
    float height{};

    [[nodiscard]] glm::vec2 dimens() const { return {width, height}; }
    [[nodiscard]] glm::vec2 pos() const { return {x, y}; }
};


#endif //SENIORRESEARCH_RECTANGLE_H
