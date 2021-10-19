//
// Created by Tobiathan on 10/8/21.
//

#include "Material.h"
#include "shaders/Shader3D.h"

Material::Material(GLfloat specularIntensity, GLfloat shininess) : specularIntensity(specularIntensity), shininess(shininess) {}

void Material::Apply(const Shader3D &shader3D) const {
    shader3D.uniformSpecularIntensity.SetFloat(specularIntensity);
    shader3D.uniformShininess.SetFloat(shininess);
}
