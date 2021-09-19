#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <cmath>

#include "src/vendor/glm/glm.hpp"
#include "src/vendor/glm/gtc/type_ptr.hpp"
#include "src/Shader.h"
#include "src/GLWindow.h"
#include "src/Mesh.h"
#include "src/Revolver.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec2 mousePos;
bool mouseDown = false;
std::vector<glm::vec2> plottedPoints;

static void mousePosCallback(GLFWwindow* window, double x, double y) {
    mousePos = {x, y};
}

static void mousePressCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    if (action == GLFW_PRESS) {
        mouseDown = true;
    } else if (action == GLFW_RELEASE) {
        mouseDown = false;
    }
}

static glm::vec2 MouseToScreen(glm::vec2 mouseVec) {
    return {mousePos.x / WIDTH * 2 - 1, (mousePos.y / HEIGHT - 0.5f) * -2};
}

static glm::vec2 MouseToScreenNorm01(glm::vec2 mouseVec) {
    return {mousePos.x / WIDTH, mousePos.y / HEIGHT};
}

// The MAIN function, from here we start the application and run the game loop
int main() {

    GLWindow window(WIDTH, HEIGHT);
    //glfwSwapInterval(0); // NOTE: Removes limit from FPS!


    Shader shader = Shader::Read("../assets/shaders/shader.vert", "../assets/shaders/shader.frag");
    Uniform uniformModel = shader.GenUniform("model");
    Uniform uniformView = shader.GenUniform("view");
    Uniform uniformProjection = shader.GenUniform("projection");

    GLfloat vertices[] = {
            -1.0f, -1.0f, 0.0f,
            0.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    };

    GLuint indices[] = {
            0, 3, 1,
            1, 3, 2,
            2, 3, 0,
            0, 1, 2
    };

    Mesh mesh(vertices, indices, sizeof vertices, sizeof indices);

    window.RegisterMouseMoveEvent(mousePosCallback);
    window.RegisterMouseButtonEvent(mousePressCallback);

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), 0.1f, 100.0f);

    // Game loop
    while (!window.ShouldClose())
    {
        // setup vars
        float time = (float) glfwGetTime();

        // Input
        auto onScreen = MouseToScreen(mousePos);
        auto normMouse = MouseToScreenNorm01(mousePos);

        // Poll Events
        glfwPollEvents();

        // >> RENDER ============================

        // Clear Background
        glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // SHADER
        shader.Enable();

        // UNIFORMS
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
        //model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

        uniformModel.SetMat4(model);
        uniformView.SetMat4(glm::mat4(1.0f));
        uniformProjection.SetMat4(projection);

        if (mouseDown) {
            plottedPoints.emplace_back(onScreen);
            mesh.Set(Revolver::Revolve(plottedPoints, 10));
        }

        // Rendering Mesh
        mesh.Render();

        // Swap the screen buffers
        window.SwapBuffers();
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}