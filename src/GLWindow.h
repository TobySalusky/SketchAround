//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_GLWINDOW_H
#define SENIORRESEARCH_GLWINDOW_H

#include <glew.h>
#include <glfw3.h>

class GLWindow {
public:
    GLWindow(GLint windowWidth, GLint windowHeight);
    ~GLWindow();

    [[nodiscard]] GLint GetBufferWidth() const { return bufferWidth; }
    [[nodiscard]] GLint GetBufferHeight() const { return bufferHeight; }
    bool ShouldClose() { return glfwWindowShouldClose(window); }
    void SwapBuffers() { glfwSwapBuffers(window); }

    void RegisterMouseMoveEvent(GLFWcursorposfun callback) { glfwSetCursorPosCallback(window, callback); }
    void RegisterMouseButtonEvent(GLFWmousebuttonfun callback) { glfwSetMouseButtonCallback(window, callback); }
private:
    GLFWwindow* window;
    GLint width, height;
    GLint bufferWidth, bufferHeight;
};


#endif //SENIORRESEARCH_GLWINDOW_H
