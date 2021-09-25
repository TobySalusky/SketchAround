//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_INPUT_H
#define SENIORRESEARCH_INPUT_H

#include <glew.h>
#include "../vendor/glm/detail/type_vec1.hpp"
#include "../vendor/glm/vec2.hpp"

class Input {
public:
    GLfloat mouseX = 0.0f, mouseY = 0.0f, lastMouseX = 0.0f, lastMouseY = 0.0f, mouseDiffX = 0.0f, mouseDiffY = 0.0f;

    bool mouseDown = false;

    void SetKey(int key, bool val);

    bool Pressed(int key) { return keysPressed[key]; }
    bool Unpressed(int key) { return keysUnpressed[key]; }

    bool Down(int key) { return keys[key]; }
    bool Up(int key) { return !keys[key]; }

    void EndUpdate();

    [[nodiscard]] glm::vec2 GetMouse() const { return glm::vec2(mouseX, mouseY); };
    [[nodiscard]] glm::vec2 GetLastMouse() const { return glm::vec2(lastMouseX, lastMouseY); };
    [[nodiscard]] glm::vec2 GetMouseDiff() const { return glm::vec2(mouseDiffX, mouseDiffY); };
private:
    bool keys[1024]{};
    bool keysPressed[1024]{};
    bool keysUnpressed[1024]{};
};


#endif //SENIORRESEARCH_INPUT_H
