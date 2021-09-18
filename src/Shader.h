//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_SHADER_H
#define SENIORRESEARCH_SHADER_H

#include "Uniform.h"

class Shader {
public:
    unsigned int ID;

    Shader();
    ~Shader();

    void Enable();
    Uniform GenUniform(const char* identifier);
};


#endif //SENIORRESEARCH_SHADER_H
