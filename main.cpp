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
#include "src/gl/Normals.h"
#include "src/generation/Sampler.h"
#include "src/gl/Texture.h"
#include "src/gl/Mesh2D.h"
#include "src/gl/shaders/Shader2D.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
bool cameraMode = false;
float lastTime = 0.0f;
float scaleRadius = 1.0f;

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

    // 3D-SHADER
    Uniform uniformModel = shader.GenUniform("model"); // TODO: abstract uniforms to Shader3D > ShaderPhong classes
    Uniform uniformView = shader.GenUniform("view");
    Uniform uniformProjection = shader.GenUniform("projection");
    Uniform uniformAmbientColor = shader.GenUniform("directionalLight.color");
    Uniform uniformAmbientIntensity = shader.GenUniform("directionalLight.ambientIntensity");
    Uniform uniformDirection = shader.GenUniform("directionalLight.direction");
    Uniform uniformDiffuseIntensity = shader.GenUniform("directionalLight.diffuseIntensity");

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

    auto UpdateMesh = [&]() {
        if (!plottedPoints.empty()) {
            const auto sampled = Sampler::DumbSample(plottedPoints, 0.1f);
            std::vector<glm::vec2> translatedPoints;
            translatedPoints.reserve(sampled.size());
            for (auto& vec : sampled) {
                translatedPoints.emplace_back(glm::vec2(vec.x, (vec.y + 0.95f) * scaleRadius));
            }
            mesh.Set(Revolver::Revolve(translatedPoints, 10));
        } else {
            mesh.Set(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));
        };
    };

    // >> FRAME BUFFER TESTING!!! TODO: try buffer width/height

    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);

    GLuint textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);



    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("problem setting up framebuffer");
    }

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

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glEnable(GL_DEPTH_TEST);

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

        mainLight.UseLight(uniformAmbientIntensity, uniformAmbientColor, uniformDirection, uniformDiffuseIntensity);
        uniformModel.SetMat4(model);
        uniformView.SetMat4(camera.CalculateViewMat());
        uniformProjection.SetMat4(projection);

        // Rendering Mesh

        //frameBuffer.Bind();
        mesh.Render();
        //frameBuffer.Unbind();

        shader.Disable();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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
        shader2D.SetColor(lineColor); // >> MODEL 2D

        linePlot.AddLines(plottedPoints, 0.01f); // FIXME: this is wrecking performance!
        linePlot.ImmediateClearingRender();

        shader2D.Disable();

        // >> ===================================


        // >> ImGui Render ====================
        ImGuiHelper::BeginFrame();


        ImGui::Begin("TEST");
        ImGui::Text("pointer = %u", textureColorBuffer);
        ImGui::Text("size = %d x %d", WIDTH, HEIGHT);
        ImGui::Image((void*)(intptr_t)textureColorBuffer, {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
        ImGui::End();



        ImGui::Begin("General");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::ColorEdit3("model", mainLight.ColorPointer());
        ImGui::ColorEdit3("lines", (float*) &lineColor);

        ImGui::SliderFloat("scale-radius", &scaleRadius, 0.1f, 3.0f);
        if (ImGui::IsItemActive()) {
            UpdateMesh();
        }

        ImGui::End();


        ImGuiHelper::EndFrame();
        // ====================================


        // UPDATE MESH
        if (!window.IsImGuiUsingMouse() && input->mouseDown) {
            if (plottedPoints.empty() || plottedPoints[plottedPoints.size() - 1] != onScreen) {
                plottedPoints.emplace_back(onScreen);
                UpdateMesh();

                glm::vec newCameraPos = camera.GetPos();
                newCameraPos.x = onScreen.x;
                camera.SetPos(newCameraPos);
            }
        }


        // Swap the screen buffers
        window.SwapBuffers();

        // TESTING
        {
            if (input->Pressed(GLFW_KEY_N)) {
//                std::vector<glm::vec3> vertices;
//                std::vector<unsigned int> indices;
//                tie(vertices, indices) = Revolver::Revolve(plottedPoints, 3);
//                Normals::Define(vertices, indices);
                Normals::Define(vertices, indices, sizeof(vertices) / sizeof(GLfloat), sizeof(indices) / sizeof(GLuint));
            }

            if (input->Pressed(GLFW_KEY_M)) {
                printf("init: %lu\n", plottedPoints.size());
                printf("new: %lu\n", Sampler::DumbSample(plottedPoints, 0.1f).size());
            }

            if (input->Pressed(GLFW_KEY_X)) {
                plottedPoints.clear();
                UpdateMesh();
            }

        }

        // ending stuff
        lastTime = time;
    }


    // Terminate GLFW, clearing any resources allocated by GLFW.
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}