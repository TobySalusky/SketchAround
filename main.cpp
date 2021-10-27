#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

#include "src/vendor/imgui/imgui.h"
#include "src/vendor/glm/glm.hpp"

#define GLM_SWIZZLE
#include "src/vendor/glm/gtc/type_ptr.hpp"

#include "src/vendor/glm/gtx/vec_swizzle.hpp"
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
#include "src/graphing/Function.h"
#include "src/graphing/GraphView.h"
#include "src/generation/Lathe.h"

// enums
enum DrawMode {
    MODE_PLOT, MODE_GRAPH_Y, MODE_GRAPH_Z
};

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
DrawMode drawMode = MODE_PLOT;
bool cameraMode = false;
float lastTime = 0.0f;
float scaleRadius = 1.0f, scaleZ = 1.0f, scaleY = 1.0f, leanScalar = 0.25f;
int countPerRing = 10;
float sampleLength = 0.1f;

bool graphFocused;

std::vector<Lathe> lathes {Lathe()};
Lathe* lathe; // TODO: use unique_ptr?? -- sus...

GraphView graphView {-1.0f, 1.0f, -1.0f, 0.0f};

glm::vec2 MouseToScreen(glm::vec2 mouseVec) {
    return {mouseVec.x / WIDTH * 2 - 1, (mouseVec.y / HEIGHT - 0.5f) * -2};
}

glm::vec2 MouseToScreenNorm01(glm::vec2 mouseVec) {
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

    Light mainLight{{0.5f, 0.5f, 0.5f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 0.8f};

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



    Mesh2D plot;

    glm::vec4 lineColor = {1.0f, 0.0f, 0.0f, 1.0f};
    glm::vec4 graphColorY = {0.0f, 0.0f, 1.0f, 1.0f};
    glm::vec4 graphColorZ = {0.0f, 1.0f, 0.0f, 1.0f};

    auto UpdateMesh = [&]() {
        if (!lathe->plottedPoints.empty()) {
            const auto sampled = Sampler::DumbSample(lathe->plottedPoints, sampleLength);
            mesh.Set(Revolver::Revolve(sampled, {
                .scaleRadius=scaleRadius,
                .scaleZ=scaleZ,
                .scaleY=scaleY,
                .countPerRing=countPerRing,
                .leanScalar=leanScalar,
                .graphYPtr=&(lathe->graphedPointsY),
                .graphZPtr=&(lathe->graphedPointsZ),
            }));
        } else {
            mesh.Set(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));
        };
    };
    const auto SetLathe = [&](Lathe* lathePtr) {
        lathe = lathePtr;
        UpdateMesh();
    };

    RenderTarget modelScene{window.GetBufferWidth(), window.GetBufferHeight(), true};
    RenderTarget graphScene{window.GetBufferWidth(), window.GetBufferHeight()};

    Material material {0.8f, 16};

    SetLathe(&lathes[0]);

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

        auto onScreen = (MouseToScreen(input->GetMouse()) + glm::vec2(0.5f, 0.5f)) * 2.0f;
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

        mainLight.SetColor(lathe->color);
        mainLight.Apply(shader3D);
        material.Apply(shader3D);
        shader3D.SetModel(model);
        shader3D.SetView(camera.CalculateViewMat());
        shader3D.SetProjection(projection);
        shader3D.SetCameraPos(camera.GetPos());

        mesh.Render();

        /*model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.5f));
        shader3D.SetModel(model);
        mesh.Render();*/

        shader.Disable();

        RenderTarget::Unbind();
        RenderTarget::Bind(graphScene);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // >> MODEL 2D ==========================
        shader2D.Enable();

        shader2D.SetModel(graphView.GenProjection());

        plot.AddQuad({-1.0f, 0.0f}, {1.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
        plot.AddQuad({-1.0f, -0.003f}, {1.0f, 0.003f}, {0.2f, 0.2f, 0.2f, 1.0f});
        plot.AddQuad({-0.003f, 1.0f}, {0.003f, -1.0f}, {0.2f, 0.2f, 0.2f, 1.0f});

        const auto SelectOpacity = [&](DrawMode desiredDrawMode, glm::vec3 color) {
            return glm::vec4(color, drawMode == desiredDrawMode ? 1.0f : 0.3f);
        };
        // FIXME: this is wrecking performance!
        // TODO: why is opacity not changing??
        plot.AddLines(lathe->graphedPointsY, SelectOpacity(MODE_GRAPH_Y, graphColorY));
        plot.AddLines(lathe->graphedPointsZ, SelectOpacity(MODE_GRAPH_Z, graphColorZ));
        plot.AddLines(lathe->plottedPoints, SelectOpacity(MODE_PLOT, lineColor));

        plot.ImmediateClearingRender();

        shader2D.Disable();

        RenderTarget::Unbind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // >> ===================================


        // >> ImGui Render ====================
        ImGuiHelper::BeginFrame();


        ImGui::Begin("Model Scene");
        ImGui::Image((void *) (intptr_t) modelScene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f},
                     {1.0f, 0.0f});
        ImGui::End();

        ImGui::Begin("Graph Scene");
        ImGui::ImageButton((void *) (intptr_t) graphScene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f},
                           {1.0f, 0.0f});
        //graphFocused = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
        graphFocused = ImGui::IsItemActive();
        ImGui::End();

        ImGui::Begin("General");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        if (ImGui::CollapsingHeader("Colors")) {
            ImGui::ColorEdit3("model", (float *) &lathe->color);
            ImGui::ColorEdit3("lines", (float *) &lineColor);
            ImGui::ColorEdit3("graph-y", (float *) &graphColorY);
            ImGui::ColorEdit3("graph-z", (float *) &graphColorZ);
        }

        if (ImGui::CollapsingHeader("Graph View")) {
            ImGui::SliderFloat("minX", &graphView.minX, -10.0f, 0.0f);
            ImGui::SliderFloat("maxX", &graphView.maxX, 0.0f, 10.0f);
            ImGui::SliderFloat("minY", &graphView.minY, -10.0f, 0.0f);
            ImGui::SliderFloat("maxY", &graphView.maxY, 0.0f, 10.0f);
        }

        const auto BindUIMeshUpdate = [&]() {
            if (ImGui::IsItemActive())
                UpdateMesh();
        };

        ImGui::SliderFloat("scale-radius", &scaleRadius, 0.1f, 3.0f);
        BindUIMeshUpdate();

        ImGui::SliderFloat("scale-z", &scaleZ, 0.1f, 3.0f);
        BindUIMeshUpdate();

        ImGui::SliderFloat("scale-y", &scaleY, 0.1f, 3.0f);
        BindUIMeshUpdate();

        ImGui::SliderFloat("lean-scalar", &leanScalar, 0.0f, 1.0f);
        BindUIMeshUpdate();

        ImGui::SliderInt("count-per-ring", &countPerRing, 3, 40);
        BindUIMeshUpdate();

        ImGui::SliderFloat("sample-length", &sampleLength, 0.01f, 0.5f);
        BindUIMeshUpdate();

        ImGui::End();

        // MODE window
        ImGui::Begin("Mode");
        {
            const auto modeSet = [&](const char* title, DrawMode newDrawMode, std::vector<glm::vec2>& clearableVec) {
                if (drawMode == newDrawMode) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
                else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});
                if (ImGui::Button(title)) drawMode = newDrawMode;
                ImGui::PopStyleColor(1);

                ImGui::SameLine();
                if (ImGui::Button((std::string("X##") + title).c_str())) {
                    clearableVec.clear();
                    UpdateMesh();
                }
            };
            modeSet("Plot", MODE_PLOT, lathe->plottedPoints);
            modeSet("Graph-Y", MODE_GRAPH_Y, lathe->graphedPointsY);
            modeSet("Graph-Z", MODE_GRAPH_Z, lathe->graphedPointsZ);
        }
        ImGui::End();

        ImGui::Begin("Models");
        {
            for (int i = 0; i < lathes.size(); i++) {
                bool isCurrentLathe = lathe == &lathes[i];
                if (isCurrentLathe) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
                else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});

                if (ImGui::Button(std::to_string(i).c_str(), { ImGui::GetWindowContentRegionWidth() - 19.0f, 24.0f})) {
                    SetLathe(&lathes[i]);
                }

                ImGui::PopStyleColor(1);

                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 9.0f);
                if (ImGui::Button((std::string("X##") + std::to_string(i)).c_str()) && lathes.size() > 1) {
                    lathes.erase(lathes.begin() + i);
                    if (isCurrentLathe) {
                        SetLathe(&lathes[i == lathes.size() ? i - 1 : i]);
                    }
                    else if (&lathes[i] < lathe) {
                        SetLathe(lathe - 1);
                    }
                }
            }
            if (ImGui::Button("+", {ImGui::GetWindowContentRegionWidth(), 24.0f})) {
                lathes.emplace_back(Lathe());
                SetLathe(&lathes[lathes.size() - 1]);
            }
        }
        ImGui::End();


        ImGuiHelper::EndFrame();
        // ====================================


        // UPDATE MESH
        if (graphFocused && input->mouseDown) {
            switch (drawMode) {
                case MODE_PLOT:
                    if (lathe->plottedPoints.empty() || lathe->plottedPoints[lathe->plottedPoints.size() - 1] != onScreen) {
                        lathe->plottedPoints.emplace_back(onScreen);
                        //printf("%f\n", onScreen.x);
                        UpdateMesh();

                        glm::vec newCameraPos = camera.GetPos();
                        newCameraPos.x = onScreen.x;
                        camera.SetPos(newCameraPos);
                    }
                    break;
                case MODE_GRAPH_Z:
                case MODE_GRAPH_Y:
                    std::vector<glm::vec2>& vec = drawMode == MODE_GRAPH_Y ? lathe->graphedPointsY : lathe->graphedPointsZ;
                    if (vec.empty() || onScreen.x > vec.back().x) {
                        vec.emplace_back(onScreen);
                        UpdateMesh();
                    }
                    break;
            }
        }


        // Swap the screen buffers
        window.SwapBuffers();

        if (input->Pressed(GLFW_KEY_X)) {
            lathe->plottedPoints.clear();
            lathe->graphedPointsY.clear();
            lathe->graphedPointsZ.clear();
            UpdateMesh();
        }

        if (input->Down(GLFW_KEY_LEFT_SHIFT)) {
            if (input->Pressed(GLFW_KEY_UP)) {
                if (lathe == &lathes[0]) {
                    SetLathe(&lathes[lathes.size() - 1]);
                } else {
                    SetLathe(lathe - 1);
                }
            }

            if (input->Pressed(GLFW_KEY_DOWN)) {
                if (lathe == &lathes[lathes.size() - 1]) {
                    SetLathe(&lathes[0]);
                } else {
                    SetLathe(lathe + 1);
                }
            }
        }

        // ending stuff
        lastTime = time;
    }


    // Manual Resource Clears
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}