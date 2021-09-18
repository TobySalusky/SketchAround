//
// Created by Tobiathan on 9/18/21.
//

#include <cstdio>

#include <glew.h>
#include <glfw3.h>

#include "GLWindow.h"

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) {
    width = windowWidth;
    height = windowHeight;

    // Init GLFW
    if (!glfwInit()) {
        printf("Error Initialising GLFW");
        glfwTerminate();
    }
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    window = glfwCreateWindow(width, height, "Senior Research Project", nullptr, nullptr);

    glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight);

    if (window == nullptr)
    {
        printf("Failed to create GLFW window");
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW");
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    glEnable(GL_DEPTH_TEST);

    // Define the viewport dimensions
    glViewport(0, 0, bufferWidth, bufferHeight);
}

GLWindow::~GLWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
