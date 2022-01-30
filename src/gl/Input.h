//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_INPUT_H
#define SENIORRESEARCH_INPUT_H

#include <glew.h>
#include "../vendor/glm/detail/type_vec1.hpp"
#include "../vendor/glm/vec2.hpp"
#include <functional>

class Input {
public:
    float mouseScroll = 0.0f;

    float mouseX = 0.0f, mouseY = 0.0f, lastMouseX = 0.0f, lastMouseY = 0.0f, mouseDiffX = 0.0f, mouseDiffY = 0.0f;

    bool mouseDown = false, mousePressed = false, mouseUnpressed = false;
    bool mouseRightDown = false, mouseRightPressed = false, mouseRightUnpressed = false;
    bool mouseMiddleDown = false, mouseMiddlePressed = false, mouseMiddleUnpressed = false;

    void SetKey(int key, bool val);

    [[nodiscard]] bool Pressed(int key) const { return keysPressed[key]; }
    [[nodiscard]] bool Unpressed(int key) const { return keysUnpressed[key]; }

    [[nodiscard]] bool Down(int key) const { return keys[key]; }
    [[nodiscard]] bool Up(int key) const { return !keys[key]; }

    void EndUpdate();

    [[nodiscard]] glm::vec2 GetMouse() const { return glm::vec2(mouseX, mouseY); };
    [[nodiscard]] glm::vec2 GetLastMouse() const { return glm::vec2(lastMouseX, lastMouseY); };
    [[nodiscard]] glm::vec2 GetMouseDiff() const { return glm::vec2(mouseDiffX, mouseDiffY); };

    void UsePressedCallback(const std::function<void(int)>& keyFunc);

private:
    bool keys[1024]{};
    bool keysPressed[1024]{};
    bool keysUnpressed[1024]{};
};


#endif //SENIORRESEARCH_INPUT_H
