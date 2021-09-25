//
// Created by Tobiathan on 9/18/21.
//

#include <glew.h>
#include <glfw3.h>
#include <iostream>

#include "Camera.h"
#include "../vendor/glm/ext/matrix_transform.hpp"

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

void Camera::Update(float deltaTime, Input *input) {

    // mouse input
    yaw += turnSpeed * input->mouseDiffX;
    pitch -= turnSpeed * input->mouseDiffY;
    pitch = std::clamp(pitch, -(float)M_PI_2 * 0.999f, (float)M_PI_2 * 0.999f);

    CalculateDir();

    // key input
    glm::vec3 dir{0.0f, 0.0f, 0.0f};
    glm::vec3 vertical{0.0f, 0.0f, 0.0f};

    if (input->Down(GLFW_KEY_W)) dir += front;
    if (input->Down(GLFW_KEY_S)) dir -= front;
    if (input->Down(GLFW_KEY_A)) dir -= right;
    if (input->Down(GLFW_KEY_D)) dir += right;

    if (glm::length(dir) > 0) dir = glm::normalize(dir);

    if (input->Down(GLFW_KEY_SPACE)) vertical += worldUp;
    if (input->Down(GLFW_KEY_LEFT_SHIFT)) vertical -= worldUp;

    pos += (dir + vertical) * movementSpeed * deltaTime;
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
