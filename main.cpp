#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "src/vendor/imgui/imgui.h"

#include "src/vendor/glm/glm.hpp"
#include "src/vendor/glm/gtc/type_ptr.hpp"
#include "src/gl/shaders/Shader.h"
#include "src/gl/GLWindow.h"
#include "src/gl/Mesh.h"
#include "src/generation/Revolver.h"
#include "src/util/ImGuiHelper.h"
#include "src/gl/Camera.h"
#include "src/gl/Light.h"
#include "src/generation/Sampler.h"
#include "src/gl/Texture.h"
#include "src/gl/Mesh2D.h"
#include "src/gl/shaders/Shader2D.h"
#include "src/gl/RenderTarget.h"
#include "src/gl/Material.h"
#include "src/gl/Function.h"

// enums
enum DrawMode {
    MODE_PLOT, MODE_GRAPH
};

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
DrawMode drawMode = MODE_PLOT;
bool cameraMode = false;
float lastTime = 0.0f;
float scaleRadius = 1.0f;
int countPerRing = 10;
float sampleLength = 0.1f;

bool graphFocused;

std::vector<glm::vec2> plottedPoints;
std::vector<glm::vec2> graphedPoints;


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

    // 3D-SHADER
    Shader3D shader3D = Shader3D::Read("../assets/shaders/shader.vert", "../assets/shaders/shader.frag");

    // 2D-SHADER
    Shader2D shader2D = Shader2D::Read("../assets/shaders/shader2D.vert", "../assets/shaders/shader2D.frag");
    Shader2D shader2D_background = Shader2D::Read("../assets/shaders/shader2D.vert", "../assets/shaders/shader2D.frag");
    Shader2D shader2D_axis = Shader2D::Read("../assets/shaders/shader2D.vert", "../assets/shaders/shader2D.frag");

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

    Mesh mesh(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));

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

    Texture texture{"../assets/images/test.png"};



    Mesh2D linePlot;
    Mesh2D background;
    Mesh2D axis;
    background.AddQuad({-1.0f, 0.0f}, {1.0f, -1.0f});
    axis.AddQuad({-1.0f, -0.945f}, {1.0f, -0.95f});

    glm::vec4 lineColor = {1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 auxLineColor = {0.0f, 0.0f, 1.0f, 1.0f};

    auto UpdateMesh = [&]() {
        if (!plottedPoints.empty()) {
            const auto sampled = Sampler::DumbSample(plottedPoints, sampleLength);
            std::vector<glm::vec2> translatedPoints;
            translatedPoints.reserve(sampled.size());
            for (auto& vec : sampled) {
                translatedPoints.emplace_back(glm::vec2(vec.x, (vec.y + 0.95f) * scaleRadius));
            }
            std::vector<glm::vec2> translatedGraph;
            translatedGraph.reserve(graphedPoints.size());
            for (auto& vec : graphedPoints) {
                translatedGraph.emplace_back(glm::vec2(vec.x, (vec.y + 0.95f)));
            }
            mesh.Set(Revolver::Revolve(translatedPoints, countPerRing, &translatedGraph));
        } else {
            mesh.Set(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));
        };
    };

    RenderTarget modelScene{window.GetBufferWidth(), window.GetBufferHeight(), true};
    RenderTarget graphScene{window.GetBufferWidth(), window.GetBufferHeight() / 2};

    Material material {1.0f, 32};

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

        RenderTarget::Bind(modelScene);

        // Clear Background

        //glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // >> 3D RENDERING =================

        // SHADER
        shader.Enable();

        // UNIFORMS
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));

        material.Apply(shader3D);
        mainLight.Apply(shader3D);
        camera.Apply(shader3D);
        shader3D.SetModel(model);
        shader3D.SetView(camera.CalculateViewMat());
        shader3D.SetProjection(projection);

        mesh.Render();

        shader.Disable();

        RenderTarget::Unbind();
        RenderTarget::Bind(graphScene);

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // >> MODEL 2D ==========================
        shader2D_background.Enable();
        shader2D_background.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
        background.ImmediateRender();
        shader2D_background.Disable();

        shader2D_background.Enable();
        shader2D_background.SetColor({0.2f, 0.2f, 0.2f, 1.0f});
        axis.ImmediateRender();
        shader2D_background.Disable();

        shader2D.Enable();
        shader2D.SetColor(auxLineColor); // >> MODEL 2D

        linePlot.AddLines(graphedPoints, 0.01f); // FIXME: this is wrecking performance!
        linePlot.ImmediateClearingRender();

        shader2D.Disable();

        shader2D.Enable();
        shader2D.SetColor(lineColor); // >> MODEL 2D

        linePlot.AddLines(plottedPoints, 0.01f); // FIXME: this is wrecking performance!
        linePlot.ImmediateClearingRender();

        shader2D.Disable();

        RenderTarget::Unbind();

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // >> ===================================


        // >> ImGui Render ====================
        ImGuiHelper::BeginFrame();


        ImGui::Begin("Model Scene");
        ImGui::Image((void *) (intptr_t) modelScene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f},
                     {1.0f, 0.0f});
        ImGui::End();

        ImGui::Begin("Graph Scene");
        ImGui::ImageButton((void *) (intptr_t) graphScene.GetTexture(), {WIDTH, HEIGHT / 2.0f}, {0.0f, 1.0f},
                           {1.0f, 0.0f});
        //graphFocused = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
        graphFocused = ImGui::IsItemActive();
        ImGui::End();

        ImGui::Begin("General");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::ColorEdit3("model", mainLight.ColorPointer());
        ImGui::ColorEdit3("lines", (float *) &lineColor);
        ImGui::ColorEdit3("graphs", (float *) &auxLineColor);

        ImGui::SliderFloat("scale-radius", &scaleRadius, 0.1f, 3.0f);
        if (ImGui::IsItemActive())
            UpdateMesh();

        ImGui::SliderInt("count-per-ring", &countPerRing, 3, 40);
        if (ImGui::IsItemActive())
            UpdateMesh();

        ImGui::SliderFloat("sample-length", &sampleLength, 0.01f, 0.5f);
        if (ImGui::IsItemActive())
            UpdateMesh();

        ImGui::End();

        // MODE window
        ImGui::Begin("Mode");
        {
            if (drawMode == MODE_PLOT) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
            else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});
            if (ImGui::Button("Plot")) drawMode = MODE_PLOT;
            ImGui::PopStyleColor(1);

            if (drawMode == MODE_GRAPH) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
            else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});

            if (ImGui::Button("Graph")) drawMode = MODE_GRAPH;
            ImGui::PopStyleColor(1);
        }
        ImGui::End();


        ImGuiHelper::EndFrame();
        // ====================================


        // UPDATE MESH
        if (graphFocused && input->mouseDown) {
            switch (drawMode) {
                case MODE_PLOT:
                    if (plottedPoints.empty() || plottedPoints[plottedPoints.size() - 1] != onScreen) {
                        plottedPoints.emplace_back(onScreen);
                        UpdateMesh();

                        glm::vec newCameraPos = camera.GetPos();
                        newCameraPos.x = onScreen.x;
                        camera.SetPos(newCameraPos);
                    }
                    break;
                case MODE_GRAPH:
                    if (graphedPoints.empty() || onScreen.x > graphedPoints.back().x) {
                        graphedPoints.emplace_back(onScreen);
                        UpdateMesh();
                    }
                    break;
            }
        }


        // Swap the screen buffers
        window.SwapBuffers();

        if (input->Pressed(GLFW_KEY_X)) {
            plottedPoints.clear();
            graphedPoints.clear();
            UpdateMesh();
        }

        // ending stuff
        lastTime = time;
    }


    // Manual Resource Clears
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}