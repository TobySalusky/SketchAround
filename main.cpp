#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "src/vendor/imgui/imgui.h"

#include "src/vendor/glm/glm.hpp"
#include "src/vendor/glm/gtc/type_ptr.hpp"
#include "src/Shader.h"
#include "src/GLWindow.h"
#include "src/Mesh.h"
#include "src/Revolver.h"
#include "src/ImGuiHelper.h"
#include "src/Camera.h"
#include "src/Light.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
bool cameraMode = false;
float lastTime = 0.0f;
bool imguiFocus = false;

std::vector<glm::vec2> plottedPoints;


static glm::vec2 MouseToScreen(glm::vec2 mouseVec) {
    return {mouseVec.x / WIDTH * 2 - 1, (mouseVec.y / HEIGHT - 0.5f) * -2};
}

static glm::vec2 MouseToScreenNorm01(glm::vec2 mouseVec) {
    return {mouseVec.x / WIDTH, mouseVec.y / HEIGHT};
}

// The MAIN function, from here we start the application and run the game loop
int main() {

    GLWindow window(WIDTH, HEIGHT);
    Input* input = window.GetInput();
    glfwSwapInterval(0); // NOTE: Removes limit from FPS!

    ImGuiHelper::Initialize(window);

    Shader shader = Shader::Read("../assets/shaders/shader.vert", "../assets/shaders/shader.frag");
    Uniform uniformModel = shader.GenUniform("model");
    Uniform uniformView = shader.GenUniform("view");
    Uniform uniformProjection = shader.GenUniform("projection");
    Uniform uniformAmbientColor = shader.GenUniform("directionalLight.color");
    Uniform uniformAmbientIntensity = shader.GenUniform("directionalLight.ambientIntensity");
    Uniform uniformDirection = shader.GenUniform("directionalLight.direction");
    Uniform uniformDiffuseIntensity = shader.GenUniform("directionalLight.diffuseIntensity");

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

    Camera camera{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -M_PI_2, 0};

    Light mainLight{{1.0f, 1.0f, 0.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, 1.0f};

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), 0.1f, 100.0f);

    auto SetCameraMode = [&](bool val) {
        cameraMode = val;
        if (cameraMode)
            window.EnableCursor();
        else
            window.DisableCursor();
    };

    SetCameraMode(false);

    while (!window.ShouldClose()) // >> UPDATE LOOP ======================================
    {
        if (input->Down(GLFW_KEY_ESCAPE)) window.Close();
        if (input->Pressed(GLFW_KEY_L)) {
            SetCameraMode(!cameraMode);
        }

        // setup vars
        auto time = (float) glfwGetTime();
        float deltaTime = time - lastTime;

        // Update Events
        // Input
        input->EndUpdate();
        glfwPollEvents();

        auto onScreen = MouseToScreen(input->GetMouse());
        auto normMouse = MouseToScreenNorm01(input->GetMouse());

        if (cameraMode) {
            camera.Update(deltaTime, input);
        }


        // >> OpenGL RENDER ========================

        // Clear Background
        //glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // SHADER
        shader.Enable();

        // UNIFORMS
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.5f));
        //model = glm::rotate(model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

        mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor, uniformDirection, uniformDiffuseIntensity);
        uniformModel.SetMat4(model);
        uniformView.SetMat4(camera.CalculateViewMat());
        uniformProjection.SetMat4(projection);

        // Rendering Mesh
        mesh.Render();

        // >> ImGui Render ====================

        ImGuiHelper::BeginFrame();

        ImGui::Begin("POGUE");

        imguiFocus = ImGui::IsWindowHovered() || ImGui::IsWindowFocused();

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGuiHelper::EndFrame();

        // ====================================


        // UPDATE MESH
        if (!imguiFocus && input->mouseDown) {
            plottedPoints.emplace_back(onScreen);
            mesh.Set(Revolver::Revolve(plottedPoints, 10));
        }


        // Swap the screen buffers
        window.SwapBuffers();

        // ending stuff
        lastTime = time;
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}