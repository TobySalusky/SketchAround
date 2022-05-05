//
// Created by Tobiathan on 9/18/21.
//

#include <glad.h>
#include <glfw3.h>
#include <iostream>

#include "Camera.h"
#include "../vendor/glm/ext/matrix_transform.hpp"
#include "../util/Controls.h"

Camera::Camera() {
	this->worldUp = {0.0f, 1.0f, 0.0f};
	
    ReCenter();
}

void Camera::Update(float deltaTime, const Input& input) {
    if (Controls::Check(CONTROLS_ResetCamera)) {
       ReCenter();
    }
	
	CalcPosDir();
}

glm::mat4 Camera::CalculateViewMat() {
    return glm::lookAt(pos, pos + front, up);
}

void Camera::CalculateDir() {
    // calculate front
    front = glm::normalize(-pos);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::ReCenter() {
	rho = 2.5f;
	theta = HALF_PI_F;
	phi = HALF_PI_F;
	
	CalcPosDir();
}

void Camera::CalcPos() {
	pos = Util::Spherical(theta, phi, rho);
}

void Camera::CalcPosDir() {
	CalcPos();
	CalculateDir();
}
