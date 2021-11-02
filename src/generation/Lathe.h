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
#include "../vendor/glm/ext/matrix_float4x4.hpp"
#include "../vendor/glm/ext/matrix_transform.hpp"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"


class Lathe {
public:
    Lathe() = default;

    float scaleRadius = 1.0f, scaleZ = 1.0f, scaleY = 1.0f, leanScalar = 0.25f;
    int countPerRing = 10;
    float sampleLength = 0.1f;

    bool wrapStart = true, wrapEnd = true;

    std::vector<glm::vec2> plottedPoints;
    std::vector<glm::vec2> graphedPointsY;
    std::vector<glm::vec2> graphedPointsZ;
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    glm::vec3 modelTranslation = {0.0f, 0.0f, -2.5f};

    [[nodiscard]] glm::mat4 GenModelMat() const {
        return glm::translate(glm::mat4(1.0f), modelTranslation);
    }

    void HyperParameterUI();

    void UpdateMesh();
    Mesh mesh{nullptr, nullptr, 0, 0};
private:
};


#endif //SENIORRESEARCH_LATHE_H
