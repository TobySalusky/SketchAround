//
// Created by Tobiathan on 9/18/21.
//

#include <glew.h>

#include "Uniform.h"
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"


void Uniform::SetMat4(glm::mat4 mat4) const {
    glUniformMatrix4fv(ID, 1, GL_FALSE, glm::value_ptr(mat4));
}

void Uniform::SetFloat(float f) const {
    glUniform1f(ID, f);
}
