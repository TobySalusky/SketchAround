//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_CAMERA_H
#define SENIORRESEARCH_CAMERA_H

#include <glad.h>
#include "Input.h"
#include "../vendor/glm/glm.hpp"
#include "shaders/Shader3D.h"
#include "../util/Util.h"

class Camera {
public:
    Camera(glm::vec3 pos, glm::vec3 worldUp, GLfloat yaw, GLfloat pitch);

    void Update(float deltaTime, const Input& input);

    glm::mat4 CalculateViewMat();

    void SetPos(glm::vec3 newPos) { pos = newPos; }
    glm::vec3 GetPos() const { return pos; }

    [[nodiscard]] GLfloat GetYaw() const;

    void SetYaw(GLfloat yaw);

    [[nodiscard]] GLfloat GetPitch() const;

    [[nodiscard]] Vec3 GetRight() const { return right; }
    [[nodiscard]] Vec3 GetUp() const { return up; }

    [[nodiscard]] Vec3 GetDir() const { return front; }

    void SetPitch(GLfloat pitch);

    static float ClampedPitch(float pitch) {
        return std::clamp(pitch, -(float)M_PI_2 * 0.999f, (float)M_PI_2 * 0.999f);
    }

    void ReCenter();


private:
    void CalculateDir();
    glm::vec3 pos, front, up, right, worldUp;
    GLfloat yaw, pitch, movementSpeed, turnSpeed;
};


#endif //SENIORRESEARCH_CAMERA_H
