//
// Created by Tobiathan on 11/29/21.
//

#include "Animator.h"

Animator::Animator() {
    keyFrameLayers[Enums::MODE_PLOT] = KeyFrameLayer<std::vector<glm::vec2>>();
    keyFrameLayers[Enums::MODE_GRAPH_Y] = KeyFrameLayer<std::vector<glm::vec2>>();
    keyFrameLayers[Enums::MODE_GRAPH_Z] = KeyFrameLayer<std::vector<glm::vec2>>();
    keyFrameLayers[Enums::MODE_CROSS_SECTION] = KeyFrameLayer<std::vector<glm::vec2>>();
}