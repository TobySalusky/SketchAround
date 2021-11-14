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
#include "src/generation/ModelObject.h"
#include "src/util/Util.h"
#include "src/generation/CrossSectionTracer.h"

#include "src/Enums.h"
#include "src/generation/Lathe.h"
#include "src/generation/CrossSectional.h"
#include "src/misc/Undo.h"
#include "src/gl/MeshUtil.h"
#include "src/animation/Timeline.h"
#include "src/util/Rectangle.h"

#define UNDO(a) undos.emplace_back(Undo([](Undo::State state) { a }));

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 800;
Enums::DrawMode drawMode = Enums::MODE_PLOT;
bool cameraMode = false;
float lastTime = 0.0f;

bool graphFocused = false;

glm::vec2 MouseToScreen(glm::vec2 mouseVec) {
    return {mouseVec.x / WIDTH * 2 - 1, (mouseVec.y / HEIGHT - 0.5f) * -2};
}

glm::vec2 MouseToScreenNorm01(glm::vec2 mouseVec) {
    return {mouseVec.x / WIDTH, mouseVec.y / HEIGHT};
}

// The MAIN function, from here we start the application and run the game loop
int main() {

    std::vector<Undo> undos;

    GraphView graphView {-1.0f, 1.0f, -1.0f, 0.0f};

    GLWindow window(WIDTH, HEIGHT);
    Input* input = window.GetInput();
    glfwSwapInterval(0); // NOTE: Removes limit from FPS!

    ImGuiHelper::Initialize(window);

    Shader shader = Shader::Read("../assets/shaders/shader.vert", "../assets/shaders/shader.frag");

    // 3D-SHADER
    Shader3D shader3D = Shader3D::Read("../assets/shaders/shader.vert", "../assets/shaders/shader.frag");

    // 2D-SHADER
    Shader2D shader2D = Shader2D::Read("../assets/shaders/shader2D.vert", "../assets/shaders/shader2D.frag");

    std::vector<ModelObject*> modelObjects {new Lathe()}; // TODO: release memory please (never deleted!)
    ModelObject* modelObject; // TODO: use unique_ptr?? -- sus...


    Camera camera{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -M_PI_2, 0};

    Light mainLight{{0.5f, 0.5f, 0.5f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 0.8f};
    Light line3DGizmoLight{{1.0f, 0.0f, 0.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), 0.1f, 100.0f);

    auto SetCameraMode = [&](bool val) {
        cameraMode = val;
        if (cameraMode)
            window.EnableCursor();
        else
            window.DisableCursor();
    };

    SetCameraMode(false);

    //Texture texture{"../assets/images/test.png"};

    Mesh planeGizmo {};

    Mesh2D plot;
    Rectangle plotRect;

    auto UpdateMesh = [&]() {
        modelObject->UpdateMesh();
    };
    const auto SetLathe = [&](ModelObject* lathePtr) {
        modelObject = lathePtr;
        UpdateMesh();
    };

    const auto AddSetLathe = [&]() {
        modelObjects.emplace_back(new CrossSectional());
        SetLathe(modelObjects[modelObjects.size() - 1]);
        // TODO: undo doesn't change current model index!!
        UNDO(state.modelObjects.pop_back();); // FIXME: leaking memory? (b/c modelObjects are pointers)
    };

    RenderTarget modelScene{window.GetBufferWidth(), window.GetBufferHeight(), true};
    RenderTarget graphScene{window.GetBufferWidth(), window.GetBufferHeight()};

    Material material {0.8f, 16};

    SetLathe(modelObjects[0]);


    EditingContext editContext;

    Timeline timeline = Timeline::Create(window);

    while (!window.ShouldClose()) // >> UPDATE LOOP ======================================
    {
        // setup time
        auto time = (float) glfwGetTime();
        float deltaTime = time - lastTime;

        // hotkeys
        if (input->Down(GLFW_KEY_ESCAPE)) window.Close();
        if (input->Pressed(GLFW_KEY_L)) SetCameraMode(!cameraMode);

        // Input Updating
        input->EndUpdate();
        glfwPollEvents();
        glm::vec2 onScreen = Util::NormalizeToRectNPFlipped(input->GetMouse(), plotRect);

        // Update Events Start
        if (cameraMode) camera.Update(deltaTime, input);

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
        material.Apply(shader3D);
        shader3D.SetView(camera.CalculateViewMat());
        shader3D.SetProjection(projection);
        shader3D.SetCameraPos(camera.GetPos());

        // Rendering models
        for (const auto renderModelObject : modelObjects) {
            if (!renderModelObject->IsVisible()) continue;

            renderModelObject->Render3D({shader3D, mainLight});
            if (renderModelObject == modelObject) renderModelObject->RenderGizmos3D({shader3D, mainLight});
        }

        // 3D Gizmos
        {
            /*line3DGizmoLight.Apply(shader3D);
            planeGizmo.Set(MeshUtil::Square({onScreen.x, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 2.0f));
            planeGizmo.Render3D();*/
        }

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

        modelObject->Render2D({plot, drawMode, onScreen});

        // line gizmo
        {
            std::vector<glm::vec2> gizmo{{onScreen.x, -1.0f},
                                         {onScreen.x, 1.0f}};
            float col = 0.7f;
            plot.AddLines(gizmo, {col, col, col, 1.0f}, 0.001f);
        }


        /*{
            // test animation!!
            if (modelObject->plottedPoints.size() >= 2 && modelObject->graphedPointsY.size() >= 2 && modelObject == modelObjects[0]) {
                const auto vecAnim = LineLerper::MorphPolyLine(modelObject->plottedPoints, modelObject->graphedPointsY, fmod(Util::sin01(time * 2.5f), 1.0f), 30);
                plot.AddLines(vecAnim,{1.0f, 0.5f, 0.5f, 1.0f});

                ModelObject* otherLathe = modelObjects[1];

                otherLathe->plottedPoints.clear();
                otherLathe->plottedPoints.insert(otherLathe->plottedPoints.end(), vecAnim.begin(), vecAnim.end());
                otherLathe->UpdateMesh();
            }
        }*/

        plot.ImmediateClearingRender();

        shader2D.Disable();

        RenderTarget::Unbind();

        timeline.Render(shader2D, drawMode);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // >> ===================================

        // >> TIMELINE ==========================



        // >> ===================================


        // >> ImGui Render3D ====================
        ImGuiHelper::BeginFrame();

        // 3D-view Window
        ImGui::Begin("Model Scene");
        {
            ImGui::Image((void *) (intptr_t) modelScene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f},
                         {1.0f, 0.0f});
        }
        ImGui::End();

        // Settings Window
        ImGui::Begin("General");
        {
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

            modelObject->AuxParameterUI({timeline});

            if (ImGui::CollapsingHeader("Graph View")) {
                ImGui::SliderFloat("minX", &graphView.minX, -10.0f, 0.0f);
                ImGui::SliderFloat("maxX", &graphView.maxX, 0.0f, 10.0f);
                ImGui::SliderFloat("minY", &graphView.minY, -10.0f, 0.0f);
                ImGui::SliderFloat("maxY", &graphView.maxY, 0.0f, 10.0f);
            }

            modelObject->HyperParameterUI({timeline});
        }
        ImGui::End();

        // MODE window
        ImGui::Begin("Mode");
        {
            modelObject->ModeSetUI(drawMode);
        }
        ImGui::End();

        // Model Instantiation Window
        ImGui::Begin("Models");
        {
            for (int i = 0; i < modelObjects.size(); i++) {
                bool isCurrentLathe = modelObject == modelObjects[i];
                if (isCurrentLathe) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.4f, 0.5f, 0.6f, 1.0f});
                else ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.3f, 0.4f, 1.0f});

                if (ImGui::Button(std::to_string(i).c_str(), { ImGui::GetWindowContentRegionWidth() - 39.0f, 24.0f})) {
                    SetLathe(modelObjects[i]);
                }

                ImGui::PopStyleColor(1);

                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 29.0f);
                ImGui::Checkbox((std::string("##") + std::to_string(i)).c_str(), modelObjects[i]->VisibilityPtr());

                ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 9.0f);
                if (ImGui::Button((std::string("X##") + std::to_string(i)).c_str()) && modelObjects.size() > 1) {
                    modelObjects.erase(modelObjects.begin() + i);
                    if (isCurrentLathe) {
                        SetLathe(modelObjects[i == modelObjects.size() ? i - 1 : i]);
                    }
                    else if (modelObjects[i] < modelObject) {
                        SetLathe(modelObject - 1);
                    }
                }
            }
            if (ImGui::Button("+", {ImGui::GetWindowContentRegionWidth(), 24.0f})) {
                AddSetLathe();
            }
        }
        ImGui::End();

        // Timeline window
        timeline.GUI(WIDTH, HEIGHT);

        // Graph window (must be last??)
        ImGui::Begin("Graph Scene");
        {
            ImGui::ImageButton((void *) (intptr_t) graphScene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f},
                               {1.0f, 0.0f});
            plotRect = ImGuiHelper::ItemRectRemovePadding(4.0f, 3.0f);
            // TODO: IsItemActive works perfectly for mouse, but focus works better for keyboard :/
            graphFocused = ImGui::IsItemFocused();
        }
        ImGui::End();

        ImGuiHelper::EndFrame();
        // ====================================

        // >> UPDATE MESH
        if (graphFocused && !cameraMode) {
            modelObject->EditMakeup({editContext, *input, drawMode, onScreen, camera});
        }


        // Swap the screen buffers
        window.SwapBuffers();



        if (input->Pressed(GLFW_KEY_X)) {
            if (input->Down(GLFW_KEY_LEFT_SHIFT)) {
                modelObject->ClearAll();
            } else {
                modelObject->ClearSingle(drawMode);
            }
            UpdateMesh();
        }

        const auto GetLatheIndex = [&]() {
            for (int i = 0; i < modelObjects.size(); i++) {
                if (modelObject == modelObjects[i]) return i;
            }
            return -1;
        };

        if (input->Down(GLFW_KEY_LEFT_SHIFT)) {
            if (input->Pressed(GLFW_KEY_UP)) {
                if (modelObject == modelObjects[0]) {
                    SetLathe(modelObjects[modelObjects.size() - 1]);
                } else {
                    SetLathe(modelObjects[GetLatheIndex() - 1]);
                }
            }

            if (input->Pressed(GLFW_KEY_DOWN)) {
                if (modelObject == modelObjects[modelObjects.size() - 1]) {
                    SetLathe(modelObjects[0]);
                } else {
                    SetLathe(modelObjects[GetLatheIndex() + 1]);
                }
            }
        }

        if (input->Pressed(GLFW_KEY_N)) {
            AddSetLathe();
        }
        if (input->Pressed(GLFW_KEY_P)) drawMode = Enums::MODE_PLOT;
        if (input->Pressed(GLFW_KEY_Y)) drawMode = Enums::MODE_GRAPH_Y;
        if (input->Pressed(GLFW_KEY_T)) drawMode = Enums::MODE_GRAPH_Z;

        if (input->Pressed(GLFW_KEY_Z) && input->Down(GLFW_KEY_LEFT_SHIFT)) {
            if (!undos.empty()) {
                undos.back().Apply({modelObjects});
                undos.pop_back();
            }
        }

        // POST UPDATE EVENTS
        timeline.Update({*input, deltaTime, *modelObject, drawMode});

        // ending stuff
        lastTime = time;
    }


    // Manual Resource Clears
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}