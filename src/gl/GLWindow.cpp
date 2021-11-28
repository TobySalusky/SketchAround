//
// Created by Tobiathan on 9/18/21.
//

#include <cstdio>

#include <glew.h>
#include <glfw3.h>

#include "GLWindow.h"

GLWindow::GLWindow(GLint windowWidth, GLint windowHeight) { // NOLINT(cppcoreguidelines-pro-type-member-init)
    width = windowWidth;
    height = windowHeight;

    // Init GLFW
    if (!glfwInit()) {
        printf("Error Initialising GLFW");
        glfwTerminate();
    }
    // Init all the required options for GLFW
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

    // Init this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
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

    CreateInput();
}

GLWindow::~GLWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void GLWindow::CreateInput() {
    input = {};
    glfwSetWindowUserPointer(window, static_cast<void*>(this));
    glfwSetKeyCallback(window, [](GLFWwindow* rawSelf, int key, int code, int action, int mode){
        auto self = static_cast<GLWindow*>(glfwGetWindowUserPointer(rawSelf));
        if (key >= 0 && key <= 1024) {
            if (action == GLFW_PRESS)
                self->input.SetKey(key, true);
            else if (action == GLFW_RELEASE)
                self->input.SetKey(key, false);
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* rawSelf, double xDiff, double yDiff) {
        auto self = static_cast<GLWindow*>(glfwGetWindowUserPointer(rawSelf));

        self->input.mouseScroll = (float) yDiff;
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* rawSelf, double xPos, double yPos){
        auto self = static_cast<GLWindow*>(glfwGetWindowUserPointer(rawSelf));

        self->input.mouseX = (float) xPos;
        self->input.mouseY = (float) yPos;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* rawSelf, int button, int action, int mods){
        auto self = static_cast<GLWindow*>(glfwGetWindowUserPointer(rawSelf));
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                self->input.mouseDown = true;
                self->input.mousePressed = true;
            }
            else if (action == GLFW_RELEASE) {
                self->input.mouseDown = false;
                self->input.mouseUnpressed = true;
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                self->input.mouseRightDown = true;
                self->input.mouseRightPressed = true;
            }
            else if (action == GLFW_RELEASE) {
                self->input.mouseRightDown = false;
                self->input.mouseRightUnpressed = true;
            }
        }
    });
}

void GLWindow::EnableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GLWindow::DisableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
