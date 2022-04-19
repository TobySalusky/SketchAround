//
// Created by Tobiathan on 9/19/21.
//

#ifndef SENIORRESEARCH_LIGHT_H
#define SENIORRESEARCH_LIGHT_H

#include <glad.h>

#define GLM_SWIZZLE

#include "../vendor/glm/glm.hpp"

#include "Uniform.h"
#include "shaders/Shader3D.h"

class Light {
public:
    Light();
    Light(glm::vec4 ambience, glm::vec3 direction, GLfloat diffuseIntensity);

    void UseLight(Uniform uniformAmbientIntensity, Uniform uniformAmbientColor, Uniform uniformDirection, Uniform uniformDiffuseIntensity);

    void SetColor(glm::vec3 color) { this->color = color; }

    void Apply(const Shader3D &shader3D);

private:
    glm::vec3 color;
    GLfloat ambientIntensity;

    glm::vec3 direction;
    GLfloat diffuseIntensity;
};


#endif //SENIORRESEARCH_LIGHT_H
