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
    Camera();

    void Update(float deltaTime, const Input& input);

    glm::mat4 CalculateViewMat();

    [[nodiscard]] Vec3 GetRight() const { return right; }
    [[nodiscard]] Vec3 GetUp() const { return up; }

    [[nodiscard]] Vec3 GetDir() const { return front; }
	
	[[nodiscard]] Vec3 GetPos() const { return pos; };
	
	static float ClampedPhi(float pitch) {
        return std::clamp(pitch, 0.0001f, PI_F * 0.9999f);
    }

    void ReCenter();

	void CalcPosDir();
	
	float rho, theta, phi;

private:
    void CalculateDir();
    glm::vec3 pos, front, up, right, worldUp;

    void CalcPos();
    
};


#endif //SENIORRESEARCH_CAMERA_H
