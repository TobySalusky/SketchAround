//
// Created by Tobiathan on 9/18/21.
//

#include <glew.h>
#include <glfw3.h>
#include <iostream>

#include "Camera.h"
#include "../vendor/glm/ext/matrix_transform.hpp"
#include "../util/Controls.h"

Camera::Camera(glm::vec3 pos, glm::vec3 worldUp, GLfloat yaw, GLfloat pitch) {
    this->pos = pos;
    this->worldUp = worldUp;
    this->yaw = yaw;
    this-> pitch = pitch;
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    movementSpeed = 15.0f;
    turnSpeed = 0.01f;

    CalculateDir();
}

void Camera::Update(float deltaTime, const Input& input) {

    if (Controls::Check(CONTROLS_ResetCamera)) {
        pitch = 0.0f;
        yaw = -M_PI_2;
        pos = {0.0f, 0.0f, 2.5f};
    }

    CalculateDir();

}

glm::mat4 Camera::CalculateViewMat() {
    return glm::lookAt(pos, pos + front, up);
}

void Camera::CalculateDir() {
    // calculate front
    front.x = cos(yaw) * cos (pitch);
    front.y = sin(pitch);
    front.z = sin(yaw) * cos(pitch);
    front = glm::normalize(front);


    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

GLfloat Camera::GetYaw() const {
    return yaw;
}

void Camera::SetYaw(GLfloat yaw) {
    Camera::yaw = yaw;
}

GLfloat Camera::GetPitch() const {
    return pitch;
}

void Camera::SetPitch(GLfloat pitch) {
    Camera::pitch = pitch;
}

void Camera::ReCenter() {
    pos = {0.0f, 0.0f, 2.5f};
    yaw = -M_PI_2;
    pitch = 0.0f;
}
