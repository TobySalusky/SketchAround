//
// Created by Tobiathan on 9/19/21.
//

#include "Light.h"
#include "Uniform.h"
#include "shaders/Shader3D.h"

Light::Light() : Light(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), 0.5f) {}

Light::Light(glm::vec4 ambience, glm::vec3 direction, GLfloat diffuseIntensity) {
    color = ambience.xyz();
    ambientIntensity = ambience.w;
    this->direction = direction;
    this->diffuseIntensity = diffuseIntensity;
}

void Light::UseLight(Uniform uniformAmbientIntensity, Uniform uniformAmbientColor, Uniform uniformDirection, Uniform uniformDiffuseIntensity) {
    uniformAmbientColor.SetFloat3(color);
    uniformAmbientIntensity.SetFloat(ambientIntensity);
    uniformDirection.SetFloat3(direction);
    uniformDiffuseIntensity.SetFloat(diffuseIntensity);
}

void Light::Apply(const Shader3D &shader3D) {
    UseLight(shader3D.uniformAmbientIntensity, shader3D.uniformAmbientColor, shader3D.uniformLightDirection, shader3D.uniformDiffuseIntensity);
}
