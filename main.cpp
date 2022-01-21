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
#include "src/util/Controls.h"
#include "src/gl/TiledTextureAtlas.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Window dimensions
const GLuint WIDTH = 1000, HEIGHT = 700;
Enums::DrawMode drawMode = Enums::MODE_PLOT;
bool cameraMode = false;
bool focusMode = false;
float lastTime = 0.0f;

bool graphFocused = false;
bool displayFocused = false;
bool enteringGuiScreen = false;
bool inSaveFileGUI = false;
bool inOpenFileGUI = false;
bool inControlsGUI = false;

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
    Controls::Initialize(input);

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

    Camera camera{{0.0f, 0.0f, 2.5f}, {0.0f, 1.0f, 0.0f}, -M_PI_2, 0};

    Light mainLight{{0.5f, 0.5f, 0.5f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 0.8f};
    Light line3DGizmoLight{{1.0f, 0.3f, 1.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), zNear, zFar);

    auto SetCameraMode = [&](bool val) {
        cameraMode = val;
        if (cameraMode)
            window.EnableCursor();
        else
            window.DisableCursor();
    };

    SetCameraMode(false);


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
    const auto SerializeScene = [&](const std::string& path) {

        printf("saving to `%s` -- ", path.c_str());
        std::ofstream ofs(path);
        boost::archive::text_oarchive oa(ofs);
        oa << Serialization(modelObjects, RenderTarget::SampleCentralSquare(modelScene, 64));
        printf("save successful!\n");
    };

    const auto DeserializeMeta = [&](const std::string& path) {

        Serialization::SetReadMetaOnly(true);
        Serialization serialization;

        std::ifstream ifs(path);
        boost::archive::text_iarchive ia(ifs);
        ia >> serialization;
        Serialization::SetReadMetaOnly(false);

        return serialization;
    };

    const auto DeSerializeScene = [&](const std::string& path) {

        printf("loading from `%s` -- ", path.c_str());
        Serialization serialization;

        std::ifstream ifs(path);
        boost::archive::text_iarchive ia(ifs);
        ia >> serialization;

        modelObjects = serialization.Deserialize();

        for (ModelObject* modelObj : modelObjects) { // Load initial meshes
            modelObj->UpdateMesh();
        }
        SetModelObject(modelObjects[0]);

        printf("load successful!\n");
    };



    const auto MouseModelsIntersectionAt = [&] (Vec2 mousePos) {
        Vec3 rayOrigin = camera.GetPos(); //+ camera.GetRight() * mousePos.x - camera.GetUp() * mousePos.y; // FIXME
        Vec3 rayDir = camera.GetDir();
        constexpr float constX = 0.73f, constY = 0.5f;
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), -mousePos.x * constX, camera.GetUp());
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), -mousePos.y * constY, camera.GetRight());

        rayDir = rotX * Vec4(rayDir, 0.0f);
        rayDir = rotY * Vec4(rayDir, 0.0f);
        rayDir = glm::normalize(rayDir);


        std::optional<MeshIntersection> meshIntersection = std::nullopt;

        for (ModelObject* modelObj : modelObjects) {
            if (!modelObj->IsVisible() || (focusMode && modelObj != modelObject)) continue;
            const auto thisIntersect = Mesh::Intersect(modelObj->GenMeshTuple(), modelObj->GenModelMat(), modelObj, {rayOrigin, rayDir});

            if (thisIntersect && (!meshIntersection || glm::length(thisIntersect->pos - rayOrigin) < glm::length(meshIntersection->pos - rayOrigin))) {
                meshIntersection = thisIntersect;
            }
        }

        return meshIntersection;
    };

    const auto MouseModelsIntersection = [&] {
        return MouseModelsIntersectionAt(Util::NormalizeToRectNP(input->GetMouse(), displayRect));
    };

    // TODO: move file gui to new file!
    const auto SaveFileGUI = [&] (float deltaTime) {
        static TimedPopup popupWarning;

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar);
        {
            if (ImGui::Button("<-----")) {
                inSaveFileGUI = false;
            }

            static char nameBuffer[256] = "";
            if (enteringGuiScreen) memset(&nameBuffer[0], 0, sizeof(nameBuffer));

            static char pathBuffer[256] = "../output";

            if (enteringGuiScreen) ImGui::SetKeyboardFocusHere();

            bool hasSaved = false;
            const auto SaveIf = [&](bool condition) {
                if (hasSaved || !condition) return;

                if (std::string(nameBuffer).empty()) {
                    popupWarning.Open("Must set file name!");
                    return;
                }
                if (std::string(pathBuffer).empty()) {
                    popupWarning.Open("Must set file path!");
                    return;
                }

                SerializeScene(std::string(pathBuffer) + "/" + std::string(nameBuffer) + ".mdl");

                inSaveFileGUI = false;
                hasSaved = true;
            };

            ImGuiHelper::SpacedSep();
            ImGui::InputTextWithHint("##Name", "name:", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            SaveIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            ImGui::InputTextWithHint("##Path", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            SaveIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            SaveIf(ImGui::Button("Save..."));

            popupWarning.Update(deltaTime);
            popupWarning.RenderGUI();
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    Texture texture{2, 2, {0,255, 0, 0, 0, 0, 255,0,0 , 0, 0, 0}};
    TiledTextureAtlas openFileTextureAtlas;
    const auto OpenFileGUI = [&] () {

        static std::string lastPath;
        static std::vector<std::string> subFolders;
        static std::vector<std::string> paths;

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("Open File", nullptr, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar);
        {
            if (ImGui::Button("<-----")) {
                inOpenFileGUI = false;
            }
            if (ImGuiHelper::HoverDelayTooltip()) {
                ImGui::SetTooltip("Escape [esc]");
            }

            static char pathBuffer[256] = "../output";

            if (enteringGuiScreen) {
                lastPath = "";
                ImGui::SetKeyboardFocusHere();
            }


            ImGuiHelper::SpacedSep();
            ImGui::InputTextWithHint("##Path", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            ImGuiHelper::SpacedSep();

            auto path = std::string(pathBuffer);
            bool pathExists = std::filesystem::exists(path) && std::filesystem::is_directory(path);

            if (path != lastPath) { // FIXME: fix error on ["/users/[username]/Desktop/lessons"]
                subFolders.clear();
                paths.clear();
                if (pathExists) {
                    for(auto& p : std::filesystem::directory_iterator(path)){
                        if (std::filesystem::is_directory(p.path())) subFolders.emplace_back(p.path());
                        if (p.path().extension() == ".mdl") paths.emplace_back(p.path());
                    }
                }
                std::vector<Serialization> metaReads;
                metaReads.reserve(paths.size());
                for (const std::string& pathStr : paths) {
                    auto& ref = metaReads.emplace_back(DeserializeMeta(pathStr));
                    const int desiredSize = 64 * 64 * 3;
                    if (ref.img.size() != desiredSize) {
                        printf("\n[Warning]: Incorrect size!\n");
                        ref.img.clear();
                        ref.img.reserve(desiredSize);
                        for (int i = 0; i < desiredSize; i++) {ref.img.emplace_back(0);}
                    }
                }

                openFileTextureAtlas.Generate(64, Linq::Select<Serialization, std::vector<unsigned char>*>(metaReads, [](Serialization& refVal){
                    return &(refVal.img);
                }));
            }

            for (auto& subFolder : subFolders) {
                ImGui::BulletText("%s", subFolder.c_str());
            }

            int childPathIndex = 0;
            for (auto& childPath : paths) {
                const auto OnPress = [&] {
                    DeSerializeScene(childPath);
                    inOpenFileGUI = false;
                };
                if (ImGui::Button(childPath.c_str())) OnPress();
                if (ImGuiHelper::HoverDelayTooltip()) ImGui::SetTooltip("%s", childPath.c_str());

                const auto& [uv1, uv2] = openFileTextureAtlas.GetCoords(childPathIndex);
                ImGui::ImageButton((void*)(intptr_t)openFileTextureAtlas.ID, {100.0f, 100.0f}, Util::ToImVec(uv1), Util::ToImVec(uv2));
                if (ImGui::IsItemClicked()) OnPress();
                if (ImGuiHelper::HoverDelayTooltip()) ImGui::SetTooltip("%s", childPath.c_str());
                childPathIndex++;
            }
            if (!pathExists) ImGui::Text("Invalid path!");

            lastPath = path;
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    const auto ControlsGUI = [&] {

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoCollapse |
                                           ImGuiWindowFlags_NoTitleBar);
        {
            if (ImGui::Button("<-----")) {
                inControlsGUI = false;
            }

            ImGuiHelper::SpacedSep();

            Controls::GUI();
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    const auto SampleSceneToTextureBytesRayTraceFlat = [&] (const int pixelSize = 32) {
        std::vector<unsigned char> textureData;

        const auto PushFloatAsByte = [&] (float f) {
            auto c = (unsigned char) (f * 255.0f);
            textureData.emplace_back(c);
        };
        const auto PushVecAsColor = [&] (Vec3 vec) {
            PushFloatAsByte(vec.x);
            PushFloatAsByte(vec.y);
            PushFloatAsByte(vec.z);
        };
        printf("s\n");

        const auto div = (float) pixelSize;
        for (int j = 0; j < pixelSize; j++) {
            for (int i = 0; i < pixelSize; i++) {
                Vec2 samplePos = {((float) i / div) * 2.0f - 1.0f, ((float) j / div) * 2.0f - 1.0f};
                if (const auto modelIntersection = MouseModelsIntersectionAt(samplePos)) {
                    PushVecAsColor(((ModelObject*)modelIntersection->obj)->GetColor());
                } else {
                    PushVecAsColor({0.0f, 0.0f, 0.0f});
                }
            }
        }
        return textureData;
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
            SerializeScene("../output/save.mdl");
            printf("save successful!\n");
        }
        if (input->Pressed(GLFW_KEY_RIGHT_BRACKET)) {
            printf("loading... ");
            DeSerializeScene("../output/save.mdl");
            printf("load successful!\n");
        }

        if (Controls::Check(CONTROLS_OpenFileSaveMenu)) {
            inSaveFileGUI = true;
            inOpenFileGUI = false;
            inControlsGUI = false;
            enteringGuiScreen = true;
        }

        if (Controls::Check(CONTROLS_OpenFileOpenMenu)) {
            inOpenFileGUI = true;
            inSaveFileGUI = false;
            inControlsGUI = false;
            enteringGuiScreen = true;
        }

        if (input->Pressed(GLFW_KEY_H) && input->Down(GLFW_KEY_LEFT_SHIFT)) {
            inControlsGUI = true;
            inOpenFileGUI = false;
            inSaveFileGUI = false;
            enteringGuiScreen = true;
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
        shader3D.SetModel(glm::mat4(1.0f)); // Reset Model Mat

        // 3D Interactivity / Gizmos
        {
            const Vec2 displayMousePos = Util::NormalizeToRectNPFlipped(input->GetMouse(), displayRect);
            if (Util::VecIsNormalizedNP(displayMousePos)) {
                const auto modelIntersection = MouseModelsIntersection();
                if (modelIntersection) {
                    // 3D normal highlighter
                    auto &[pos, norm, obj] = *modelIntersection;

                    line3DGizmoLight.Apply(shader3D);
                    planeGizmo.Set(MeshUtil::PolyLine({pos, pos + glm::normalize(norm) * 0.2f}));
                    planeGizmo.Render();

                    // 3D selection
                    static float netDrag = 0.0f;
                    if (input->mousePressed) {
                        netDrag = 0.0f;
                    } else if (input->mouseDown) {
                        netDrag += glm::length(displayMousePos - Util::NormalizeToRectNPFlipped(input->GetLastMouse(), displayRect));
                    }

                    if (input->mouseUnpressed && netDrag < 0.0001f) { // only select if user did not drag mouse
                        SetModelObject((ModelObject*)obj);
                    }
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

        ImGui::Begin("Test");
        {
            ImGui::ImageButton((void *) (intptr_t) texture.ID, {100.0f, 100.0f}, {0.0f, 1.0f},{1.0f, 0.0f});
            //ImGui::ImageButton((void *) (intptr_t) texture2.ID, {100.0f, 100.0f});
        }
        ImGui::End();

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

            if (ImGui::BeginDragDropTarget())
            {
                const auto modelIntersection = MouseModelsIntersection();

                Vec4 color = {0.0f, 0.0f, 0.0f, 1.0f};
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F)) {
                    memcpy((float*)&color, payload->Data, sizeof(float) * 3);
                    if (modelIntersection) {
                        ((ModelObject*)(modelIntersection->obj))->SetColor(color);
                    }
                } else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F)) {
                    memcpy((float*)&color, payload->Data, sizeof(float) * 4);
                    if (modelIntersection) {
                        ((ModelObject*)(modelIntersection->obj))->SetColor(color);
                    }
                } else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelObjectDrag")) {
                    IM_ASSERT(payload->DataSize == sizeof(ModelObject*));
                    ModelObject* draggedObj = *(ModelObject**)payload->Data;

                    if (modelIntersection) {
                        draggedObj->SetPos(modelIntersection->pos);
                        const auto normal = glm::normalize(modelIntersection->normal);
                        const auto temp = glm::normalize(glm::cross(normal, glm::cross(normal, {0.0f, 1.0f, 0.0f})));
                        const auto quaDir = glm::quatLookAt(normal, {0.0f, 1.0f, 0.0f});
                        const auto eulers = glm::eulerAngles(quaDir);
                        draggedObj->SetEulers(Util::DirToEuler(normal));
                    }
                }
                ImGui::EndDragDropTarget();
            }

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

        if (inSaveFileGUI) SaveFileGUI(deltaTime);
        if (inOpenFileGUI) OpenFileGUI();
        if (inControlsGUI) ControlsGUI();

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

        if (Controls::Check(CONTROLS_ClearAllLayers)) {
            modelObject->ClearAll();
            UpdateMesh();

        }
        else if (Controls::Check(CONTROLS_ClearCurrentLayer)) {
            modelObject->ClearSingle(drawMode);
            UpdateMesh();
        }

        if (input->Pressed(GLFW_KEY_Y)) {
            GLuint test = -1;
            glGenTextures(1, &test);
            printf(":::::::: %ui\n", test);
        }
        if (input->Pressed(GLFW_KEY_I)) {
            int pixelSize = 64;
//            texture.Set(pixelSize, pixelSize, SampleSceneToTextureBytesRayTraceFlat(pixelSize));
            texture.Set(pixelSize, pixelSize, RenderTarget::SampleCentralSquare(modelScene, pixelSize));
//            texture = {pixelSize, pixelSize, RenderTarget::SampleCentralSquare(modelScene, pixelSize)};
//            printf("e");
        }

        const auto GetLatheIndex = [&]() {
            for (int i = 0; i < modelObjects.size(); i++) {
                if (modelObject == modelObjects[i]) return i;
            }
            return -1;
        };

        if (Controls::Check(CONTROLS_SwitchModelUp)) {
            if (modelObject == modelObjects[0]) {
                SetModelObject(modelObjects[modelObjects.size() - 1]);
            } else {
                SetModelObject(modelObjects[GetLatheIndex() - 1]);
            }
        } else if (Controls::Check(CONTROLS_SwitchModelDown)) {
            if (modelObject == modelObjects[modelObjects.size() - 1]) {
                SetModelObject(modelObjects[0]);
            } else {
                SetModelObject(modelObjects[GetLatheIndex() + 1]);
            }
        }

        if (Controls::Check(CONTROLS_AddLathe)) {
            AddSetLathe();
        } else if (Controls::Check(CONTROLS_AddCrossSectional)) {
            AddSetCrossSectional();
        }

        if (Controls::Check(CONTROLS_SetLayerPrimary)) drawMode = Enums::MODE_PLOT;
        if (Controls::Check(CONTROLS_SetLayerSecondary)) drawMode = Enums::MODE_GRAPH_Y;
        if (Controls::Check(CONTROLS_SetLayerTertiary)) drawMode = Enums::MODE_GRAPH_Z;

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