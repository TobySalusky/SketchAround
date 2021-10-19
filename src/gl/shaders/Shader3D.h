//
// Created by Tobiathan on 10/8/21.
//

#ifndef SENIORRESEARCH_SHADER3D_H
#define SENIORRESEARCH_SHADER3D_H

#include "Shader.h"

class Shader3D : public Shader {
public:
    Shader3D(const char *vertexShaderSource, const char *fragmentShaderSource);

    void SetModel(glm::mat4 model) { uniformModel.SetMat4(model); }
    void SetView(glm::mat4 model) { uniformView.SetMat4(model); }
    void SetProjection(glm::mat4 model) { uniformProjection.SetMat4(model); }

    static Shader3D Read(const char* vertexShaderPath, const char* fragmentShaderPath);

private:
    Uniform uniformModel, uniformView, uniformProjection;
    Uniform uniformAmbientColor, uniformAmbientIntensity, uniformLightDirection, uniformDiffuseIntensity;
    Uniform uniformSpecularIntensity, uniformShininess;
    Uniform uniformCameraPosition;

    friend class Light;
    friend class Material;
    friend class Camera;
};


#endif //SENIORRESEARCH_SHADER3D_H
