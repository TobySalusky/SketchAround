//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_SHADER_H
#define SENIORRESEARCH_SHADER_H

#include "Uniform.h"

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
    ~Shader();

    static std::string ReadFile(const char* path);
    static Shader Read(const char* vertexShaderPath, const char* fragmentShaderPath);

    void Enable() const;
    Uniform GenUniform(const char* identifier) const;
};


#endif //SENIORRESEARCH_SHADER_H
