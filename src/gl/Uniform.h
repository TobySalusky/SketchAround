//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_UNIFORM_H
#define SENIORRESEARCH_UNIFORM_H

#include "../vendor/glm/glm.hpp"

class Uniform {
public:
    int ID;
    void SetMat4(glm::mat4 mat4) const;
    void SetFloat(float f) const;

    void SetFloat3(float f1, float f2, float f3) const;
    void SetFloat3(glm::vec3 vec) const;
    void SetFloat4(float f1, float f2, float f3, float f4) const;
    void SetFloat4(glm::vec4 vec) const;
};


#endif //SENIORRESEARCH_UNIFORM_H
