//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_UNIFORM_H
#define SENIORRESEARCH_UNIFORM_H

#include "vendor/glm/glm.hpp"

class Uniform {
public:
    int ID;
    void SetMat4(glm::mat4 mat4) const;
    void SetFloat(float f) const;
};


#endif //SENIORRESEARCH_UNIFORM_H
