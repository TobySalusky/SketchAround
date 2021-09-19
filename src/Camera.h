//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_CAMERA_H
#define SENIORRESEARCH_CAMERA_H

#include <glew.h>
#include "Input.h"
#include "vendor/glm/glm.hpp"

class Camera {
public:
    Camera(glm::vec3 pos, glm::vec3 worldUp, GLfloat yaw, GLfloat pitch);

    void Update(float deltaTime, Input* input);

    glm::mat4 CalculateViewMat();
private:
    void CalculateDir();
    glm::vec3 pos, front, up, right, worldUp;
    GLfloat yaw, pitch, movementSpeed, turnSpeed;
};


#endif //SENIORRESEARCH_CAMERA_H
