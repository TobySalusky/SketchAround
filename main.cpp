#include <glew.h>
#include <glfw3.h>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

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
#include "src/generation/CrossSectionTracer.h"

#include "src/util/Includes.h"
#include "src/generation/Lathe.h"
#include "src/generation/CrossSectional.h"
#include "src/misc/Undo.h"
#include "src/gl/MeshUtil.h"
#include "src/animation/Timeline.h"
#include "src/util/Rectangle.h"
#include "src/gl/Display3DContext.h"
#include "src/misc/Undos.h"
#include "src/misc/Serialization.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 700;
Enums::DrawMode drawMode = Enums::MODE_PLOT;
bool cameraMode = false;
bool focusMode = true;
float lastTime = 0.0f;

bool graphFocused = false;
bool displayFocused = false;

Vec2 MouseToScreen(Vec2 mouseVec) {
    return {mouseVec.x / WIDTH * 2 - 1, (mouseVec.y / HEIGHT - 0.5f) * -2};
}

Vec2 MouseToScreenNorm01(Vec2 mouseVec) {
    return {mouseVec.x / WIDTH, mouseVec.y / HEIGHT};
}

void DragDropModelObject() {
    ImGui::Dummy({ImGui::GetWindowContentRegionWidth(), fmax(40.0f, ImGui::GetContentRegionAvail().y)});
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelObjectDrag"))
        {
            IM_ASSERT(payload->DataSize == sizeof(ModelObject*));
            ModelObject* draggedObj = *(ModelObject**)payload->Data;
            draggedObj->UnParent();
        }
        ImGui::EndDragDropTarget();
    }
}

// The MAIN function, from here we start the application and run the game loop
int main() {
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

    const float zNear = 0.1f;
    const float zFar = 100.0f;

    Camera camera{glm::vec3(0.0f, 0.0f, 2.5f), glm::vec3(0.0f, 1.0f, 0.0f), -M_PI_2, 0};

    Light mainLight{{0.5f, 0.5f, 0.5f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 0.8f};
    Light line3DGizmoLight{{1.0f, 1.0f, 1.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), zNear, zFar);

    auto SetCameraMode = [&](bool val) {
        cameraMode = val;
        if (cameraMode)
            window.EnableCursor();
        else
            window.DisableCursor();
    };

    SetCameraMode(false);

    //Texture texture{"../assets/images/test.png"};

    BlendModes::GetManager().Init();

    Mesh planeGizmo {};

    Mesh2D plot;
    Rectangle plotRect;
    Rectangle displayRect;

    Timeline timeline = Timeline::Create(window);

    auto UpdateMesh = [&]() {
        modelObject->UpdateMesh();
    };
    const auto SetModelObject = [&](ModelObject* lathePtr) {
        modelObject = lathePtr;
        UpdateMesh();
        timeline.OnActiveModelObjectChange();
    };

    const auto AddSetLathe = [&]() {
        modelObjects.emplace_back(new Lathe());
        SetModelObject(modelObjects[modelObjects.size() - 1]);
    };

    const auto AddSetCrossSectional = [&]() {
        modelObjects.emplace_back(new CrossSectional());
        SetModelObject(modelObjects[modelObjects.size() - 1]);
    };


    std::function<void(ModelObject* obj)> addModelObjRecursive;
    addModelObjRecursive = [&](ModelObject* obj) {
        modelObjects.emplace_back(obj);
        obj->UpdateMesh();
        for (ModelObject* child : obj->GetChildren()) {
            addModelObjRecursive(child);
        }
    };

    DraggableUIInfo draggableUIInfo = {
        [&](ModelObject* obj) { return modelObject == obj; },
        [&](ModelObject* obj) { SetModelObject(obj); },
        addModelObjRecursive,
        [&](ModelObject* obj) {
            if (modelObjects.size() <= 1) return; // TODO: use count of base nodes only // FIXME !!! rewrite delete func!!!!
            obj->UnParent();
            for (int i = 0; i < modelObjects.size(); i++) {
                if (modelObjects[i] == obj) {
                    modelObjects.erase(modelObjects.begin() + i); // TODO: call delete on object! // FIXME: memory leak
                    break;
                }
            }
            if (obj == modelObject) SetModelObject(modelObjects[0]);
        }
    };

    RenderTarget modelScene{window.GetBufferWidth(), window.GetBufferHeight(), true};
    RenderTarget graphScene{window.GetBufferWidth(), window.GetBufferHeight()};

    Material material {0.8f, 16};

    SetModelObject(modelObjects[0]);


    EditingContext editContext;

    /*
     * TODO: Add animator + blend-modes to serialization
     * TODO: make float* GetFloatSetter(std::string label) on ModelObject (relink after copy or serial)
     */
    const auto SerializeScene = [&]() {

        std::ofstream ofs("../output/save.txt");
        boost::archive::text_oarchive oa(ofs);
        oa << Serialization(modelObjects);
    };

    const auto DeSerializeScene = [&]() {
        Serialization serialization;

        std::ifstream ifs("../output/save.txt");
        boost::archive::text_iarchive ia(ifs);
        ia >> serialization;

        modelObjects = serialization.Deserialize();
        SetModelObject(modelObjects[0]);
    };


    while (!window.ShouldClose()) // >> UPDATE LOOP ======================================
    {
        // setup time
        auto time = (float) glfwGetTime();
        float deltaTime = time - lastTime;

        // hotkeys
        if (input->Down(GLFW_KEY_ESCAPE)) window.Close();
        if (input->Pressed(GLFW_KEY_L)) SetCameraMode(!cameraMode);

        if (input->Pressed(GLFW_KEY_LEFT_BRACKET)) {
            printf("saving... ");
            SerializeScene();
            printf("save successful!\n");
        }
        if (input->Pressed(GLFW_KEY_RIGHT_BRACKET)) {
            printf("loading... ");
            DeSerializeScene();
            printf("load successful!\n");
        }

        // Input Updating
        input->EndUpdate();
        glfwPollEvents();
        Vec2 onScreen = Util::NormalizeToRectNPFlipped(input->GetMouse(), plotRect);

        // Update Events Start
        camera.Update(deltaTime, input, cameraMode);

        timeline.SetActiveAnimator(modelObject->GetAnimatorPtr());


        // >> OpenGL RENDER ========================

        RenderTarget::Bind(modelScene);

        // Clear Background

        //glClearColor(normMouse.x, 0.0f, normMouse.y, 1.0f);
        const float backgroundLevel3D = 0.1f;
        glClearColor(backgroundLevel3D, backgroundLevel3D, backgroundLevel3D, 1.0f);
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
            if (!renderModelObject->IsVisible() || (focusMode && renderModelObject != modelObject)) continue;

            renderModelObject->Render3D({shader3D, mainLight});
            if (renderModelObject == modelObject) renderModelObject->RenderGizmos3D({shader3D, mainLight});
        }

        // 3D Gizmos
        {
//            float axisLen = 1.0f;
//            line3DGizmoLight.Apply(shader3D);
//            planeGizmo.Set(MeshUtil::PolyLine({{0.0f, -axisLen, 0.0f}, {0.0f, axisLen, 0.0f}}));
//            planeGizmo.Render();


            {
                Vec2 mousePos = Util::NormalizeToRectNP(input->GetMouse(), displayRect);
                Vec3 rayOrigin = camera.GetPos() + camera.GetRight() * mousePos.x - camera.GetUp() * mousePos.y;

                // TODO: find offset for origin
//                Vec4 ray_clip = {mousePos.x, mousePos.y, -1.0, 1.0};
//                Vec4 ray_eye = glm::inverse(projection) * ray_clip;
//                ray_eye = {ray_eye.x, ray_eye.y, -1.0, 0.0};
//                Vec3 ray_wor = inverse(camera.CalculateViewMat()) * ray_eye;
//                ray_wor = glm::normalize(ray_wor);
//
//
//                rayOrigin += ray_wor;


                const auto temp = Mesh::Intersect(modelObject->GenMeshTuple(), {rayOrigin, camera.GetDir()});
//                printf("E"); Util::PrintVec(camera.GetPos());
//                printf("E"); Util::PrintVec(camera.GetDir());
                if (temp.has_value()) {
                    Vec3 pos = temp.value().pos;
                    Vec3 norm = temp.value().normal;
//                    printf("P"); Util::PrintVec(pos);
//                    printf("N"); Util::PrintVec(norm);

                    line3DGizmoLight.Apply(shader3D);
                    planeGizmo.Set(MeshUtil::PolyLine({pos, pos - glm::normalize(norm) * 0.2f}));
                    planeGizmo.Render();
                }
            }
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

        if (!timeline.IsPlaying()) timeline.RenderOnionSkin(plot, drawMode);

        modelObject->Render2D({plot, drawMode, onScreen});

        // line gizmo
        {
            std::vector<glm::vec2> gizmo{{onScreen.x, -1.0f},
                                         {onScreen.x, 1.0f}};
            float col = 0.0f;
            plot.AddLines(gizmo, {col, col, col, 0.15f}, 0.001f);
        }

        plot.ImmediateClearingRender();

        shader2D.Disable();

        RenderTarget::Unbind();

        timeline.Render({shader2D, drawMode, *input});

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

            Vec2 displayDimens = Util::FitRatio({WIDTH / 2.0f, HEIGHT / 2.0f},
                                                Util::ToVec(ImGui::GetWindowContentRegionMax()) - Util::ToVec(ImGui::GetWindowContentRegionMin()) - Vec2(8.0f, 6.0f));
            ImGui::SameLine((ImGui::GetWindowWidth()) / 2.0f - (displayDimens.x / 2.0f)); // TODO: remove padding, make no scroll!!

            ImGui::ImageButton((void*) (intptr_t) modelScene.GetTexture(),
                               Util::ToImVec(displayDimens),
                               {0.0f, 1.0f},
                         {1.0f, 0.0f});
            displayRect = ImGuiHelper::ItemRectRemovePadding(4.0f, 3.0f);
            displayFocused = ImGui::IsItemActive();
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
            if (ImGui::Button("+##Lathe")) {
                AddSetLathe();
            }
            ImGui::SameLine();
            if (ImGui::Button("+##CrossSectional")) {
                AddSetCrossSectional();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Focus", &focusMode);

            for (ModelObject* currModelObject : modelObjects) {
                if (!currModelObject->HasParent()) currModelObject->DraggableGUI(draggableUIInfo);
            }
            DragDropModelObject();
        }
        ImGui::End();

        // Timeline window
        timeline.GUI({WIDTH, HEIGHT, *input});

        // Graph window (must be last??)
        ImGui::Begin("Graph Scene");
        {
            Vec2 displayDimens = Util::FitRatio({WIDTH / 2.0f, HEIGHT / 2.0f},
                                                Util::ToVec(ImGui::GetWindowContentRegionMax()) - Util::ToVec(ImGui::GetWindowContentRegionMin()) - Vec2(8.0f, 6.0f));
            ImGui::SameLine((ImGui::GetWindowWidth()) / 2.0f - (displayDimens.x / 2.0f));
            ImGui::ImageButton((void *) (intptr_t) graphScene.GetTexture(), Util::ToImVec(displayDimens), {0.0f, 1.0f},
                               {1.0f, 0.0f});
            plotRect = ImGuiHelper::ItemRectRemovePadding(4.0f, 3.0f);
            // TODO: IsItemActive works perfectly for mouse, but focus works better for keyboard :/
            graphFocused = ImGui::IsItemFocused();
        }
        ImGui::End();

        ImGuiHelper::EndFrame();
        // ====================================

        // >> UPDATE MESH
        if (!cameraMode) {
            modelObject->EditMakeup({editContext, *input, drawMode, onScreen, camera, graphFocused});
        }


        // Swap the screen buffers
        window.SwapBuffers();


        Display3DContext::Update({*input, camera, displayRect, displayFocused});

        if (input->Down(GLFW_KEY_LEFT_SHIFT) && input->Pressed(GLFW_KEY_E)) {
            printf("%s", Mesh::GenOBJ(modelObject->GenMeshTuple()).c_str());
        }

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
                    SetModelObject(modelObjects[modelObjects.size() - 1]);
                } else {
                    SetModelObject(modelObjects[GetLatheIndex() - 1]);
                }
            }

            if (input->Pressed(GLFW_KEY_DOWN)) {
                if (modelObject == modelObjects[modelObjects.size() - 1]) {
                    SetModelObject(modelObjects[0]);
                } else {
                    SetModelObject(modelObjects[GetLatheIndex() + 1]);
                }
            }
        }

        if (input->Pressed(GLFW_KEY_N)) {
            if (input->Down(GLFW_KEY_LEFT_SHIFT)) {
                AddSetLathe();
            } else {
                AddSetCrossSectional();
            }
        }

        if (input->Pressed(GLFW_KEY_P)) drawMode = Enums::MODE_PLOT;
        if (input->Pressed(GLFW_KEY_Y)) drawMode = Enums::MODE_GRAPH_Y;
        if (input->Pressed(GLFW_KEY_T)) drawMode = Enums::MODE_GRAPH_Z;

        if (input->Pressed(GLFW_KEY_Z) && input->Down(GLFW_KEY_LEFT_SHIFT)) {
            //Undos::UseLast();
        }

        // POST UPDATE EVENTS
        timeline.Update({*input, deltaTime, drawMode, *modelObject, modelObjects, focusMode});
        modelObject->UnDiffAll();

        // ending stuff
        lastTime = time;
    }

    // Manual Resource Clears
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}