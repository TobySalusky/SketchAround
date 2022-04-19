//
// Created by Tobiathan on 9/18/21.
//

#ifndef SENIORRESEARCH_GLWINDOW_H
#define SENIORRESEARCH_GLWINDOW_H

#include <glad.h>
#include <glfw3.h>
#include "Input.h"
#include "../vendor/imgui/imgui.h"

class GLWindow {
public:
    GLWindow(GLint windowWidth, GLint windowHeight);
    ~GLWindow();

    [[nodiscard]] GLint GetWidth() const { return width; }
    [[nodiscard]] GLint GetHeight() const { return height; }
    [[nodiscard]] GLint GetBufferWidth() const { return bufferWidth; }
    [[nodiscard]] GLint GetBufferHeight() const { return bufferHeight; }
    [[nodiscard]] GLFWwindow* GetWindowPtr() const { return window; }
    [[nodiscard]] Input* GetInput() { return &input; }

    bool ShouldClose() { return glfwWindowShouldClose(window); }
    void Close() { glfwSetWindowShouldClose(window, GL_TRUE); }

    void SwapBuffers() { glfwSwapBuffers(window); }

    void EnableCursor();
    void DisableCursor();

    bool IsImGuiUsingMouse() { return io->WantCaptureMouse; }

    void SetImGuiIO(ImGuiIO* ioPtr) { io = ioPtr; }

    [[nodiscard]] bool HasResizeDiff() const { return resizeDiff; }

    void UnDiffResize() { resizeDiff = false; }

    void SetTitle(const char* title) { glfwSetWindowTitle(window,  title); }

private:
    GLFWwindow* window;
    GLint width, height;
    GLint bufferWidth, bufferHeight;

    Input input;

    ImGuiIO* io;

    bool resizeDiff;

    void CreateInput();
};


#endif //SENIORRESEARCH_GLWINDOW_H
