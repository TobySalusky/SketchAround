//
// Created by Tobiathan on 9/18/21.
//

#include <glew.h>

#include "Uniform.h"
#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/type_ptr.hpp"


void Uniform::SetMat4(glm::mat4 mat4) const {
    glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(mat4));
}

void Uniform::SetFloat(float f) const {
    glUniform1f(ID, f);
}

void Uniform::SetFloat3(float f1, float f2, float f3) const {
    glUniform3f(ID, f1, f2, f3);
}

void Uniform::SetFloat3(glm::vec3 vec) const {
    glUniform3f(ID, vec.x, vec.y, vec.z);
}

void Uniform::SetFloat4(float f1, float f2, float f3, float f4) const {
    glUniform4f(ID, f1, f2, f3, f4);
}

void Uniform::SetFloat4(glm::vec4 vec) const {
    SetFloat4(vec.x, vec.y, vec.z, vec.w);
}
