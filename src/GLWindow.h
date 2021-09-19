//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_GLWINDOW_H
#define SENIORRESEARCH_GLWINDOW_H

#include <glew.h>
#include <glfw3.h>
#include "Input.h"

class GLWindow {
public:
    GLWindow(GLint windowWidth, GLint windowHeight);
    ~GLWindow();

    [[nodiscard]] GLint GetBufferWidth() const { return bufferWidth; }
    [[nodiscard]] GLint GetBufferHeight() const { return bufferHeight; }
    [[nodiscard]] GLFWwindow* GetWindowPtr() const { return window; }
    [[nodiscard]] Input* GetInput() { return &input; }

    bool ShouldClose() { return glfwWindowShouldClose(window); }
    void Close() { glfwSetWindowShouldClose(window, GL_TRUE); }

    void SwapBuffers() { glfwSwapBuffers(window); }

    void EnableCursor();
    void DisableCursor();

private:
    GLFWwindow* window;
    GLint width, height;
    GLint bufferWidth, bufferHeight;

    Input input;

    void CreateInput();
};


#endif //SENIORRESEARCH_GLWINDOW_H
