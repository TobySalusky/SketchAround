//
// Created by Tobiathan on 9/25/21.
//

#include "Shader2D.h"
#include "../../util/Util.h"

Shader2D::Shader2D(const char *vertexShaderSource, const char *fragmentShaderSource) : Shader(vertexShaderSource, fragmentShaderSource),
    uniformModel(GenUniform("model"))
{
    Enable();
    SetModel(glm::mat4(1.0f));
    Disable();
}

Shader2D Shader2D::Read(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return {Util::ReadFile(vertexShaderPath).c_str(), Util::ReadFile(fragmentShaderPath).c_str()};
}