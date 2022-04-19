//
// Created by Tobiathan on 10/8/21.
//

#ifndef SENIORRESEARCH_MATERIAL_H
#define SENIORRESEARCH_MATERIAL_H

#include <glad.h>
#include "shaders/Shader3D.h"

class Material {
public:
    Material(GLfloat specularIntensity, GLfloat shininess);
    void Apply(const Shader3D& shader3D) const;
private:
    GLfloat specularIntensity, shininess;
};


#endif //SENIORRESEARCH_MATERIAL_H
