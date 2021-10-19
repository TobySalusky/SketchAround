//
// Created by Tobiathan on 10/8/21.
//

#include "Shader3D.h"
#include "../../util/Util.h"
#include "../Light.h"

Shader3D::Shader3D(const char *vertexShaderSource, const char *fragmentShaderSource) : Shader(vertexShaderSource, fragmentShaderSource),
    uniformModel(GenUniform("model")),
    uniformView(GenUniform("view")),
    uniformProjection(GenUniform("projection")),
    uniformAmbientColor(GenUniform("directionalLight.color")),
    uniformAmbientIntensity(GenUniform("directionalLight.ambientIntensity")),
    uniformLightDirection(GenUniform("directionalLight.direction")),
    uniformDiffuseIntensity(GenUniform("directionalLight.diffuseIntensity")),
    uniformSpecularIntensity(GenUniform("material.specularIntensity")),
    uniformShininess(GenUniform("material.shininess")),
    uniformCameraPosition(GenUniform("cameraPosition"))
{
    Enable();
    SetModel(glm::mat4(1.0f));
    Disable();
}

Shader3D Shader3D::Read(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return {Util::ReadFile(vertexShaderPath).c_str(), Util::ReadFile(fragmentShaderPath).c_str()};
}