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
#include "src/exporting/ObjExporter.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// Window dimensions
//const GLuint INIT_WIDTH = 1000, INIT_HEIGHT = 700;
const GLuint INIT_WIDTH = 1200, INIT_HEIGHT = 700;
Enums::DrawMode drawMode = Enums::MODE_PLOT;
bool cameraMode = false;
bool focusMode = false;
float lastTime = 0.0f;

bool graphFocused = false;
bool displayFocused = false;
bool enteringGuiScreen = false;
bool inSaveFileGUI = false;
bool inOpenFileGUI = false;
bool inExportGUI = false;
bool inControlsGUI = false;
bool developerMode = true;


bool requirePlotResize = true;
Vec2 newPlotDimens;

Enums::EditingTool selectedTool;

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
    GraphView graphView;

    GLWindow window(INIT_WIDTH, INIT_HEIGHT);
    Input* input = window.GetInput();
    Controls::Initialize(input);
    Undo::InitializeUndoers({&drawMode});

    GLuint WIDTH  = INIT_WIDTH;
    GLuint HEIGHT = INIT_HEIGHT;

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


    glm::mat4 projection;

    const auto SetupProjection = [&]() {
        projection = glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), zNear, zFar);
    };
    SetupProjection();

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

    Timeline timeline {window};

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


    const auto SerializeScene = [&](const std::string& path) {

        printf("saving to \"%s\" -- ", path.c_str());
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

        printf("loading from \"%s\" -- ", path.c_str());
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


    const auto SubGuiExitOptions = [&] {
        if (ImGui::Button("<-----") || Controls::Check(CONTROLS_ExitMenu)) {
            inOpenFileGUI = false;
            inSaveFileGUI = false;
            inControlsGUI = false;
            inExportGUI = false;
        }
        ImGuiHelper::DelayControlTooltip(CONTROLS_ExitMenu);
    };

    const auto MenuUpdate = [&] {
        editContext.DisableDrawingForClick();
    };

    // TODO: move file gui to new file!
    const auto SaveFileGUI = [&] (float deltaTime) {
        MenuUpdate();

        static TimedPopup popupWarning;

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar);
        {
            SubGuiExitOptions();

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
            ImGui::InputTextWithHint("##Name-Save", "name:", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            SaveIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            ImGui::InputTextWithHint("##Path-Save", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            SaveIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            SaveIf(ImGui::Button("Save..."));

            popupWarning.Update(deltaTime);
            popupWarning.RenderGUI();
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    const auto ExportGUI = [&] (float deltaTime) {
        MenuUpdate();

        static TimedPopup popupWarning;

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("File", nullptr, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar);
        {
            SubGuiExitOptions();

            static char exportNameBuffer[256] = "";
            if (enteringGuiScreen) memset(&exportNameBuffer[0], 0, sizeof(exportNameBuffer));

            static char pathBuffer[256] = "../output/obj";

            if (enteringGuiScreen) ImGui::SetKeyboardFocusHere();

            bool hasExported = false;
            const auto ExportIf = [&](bool condition) {
                if (hasExported || !condition) return;

                if (std::string(exportNameBuffer).empty()) {
                    popupWarning.Open("Must set file name!");
                    return;
                }
                if (std::string(pathBuffer).empty()) {
                    popupWarning.Open("Must set file path!");
                    return;
                }

                const auto ExportTo = [](const std::string& path, const std::string& content) {
                    std::ofstream f(path);
                    printf("Exporting to \"%s\" -- ", path.c_str());
                    if (!f.fail()) {
                        f << content.c_str();
                        printf("export successful!\n");
                    } else {
                        printf("export failed!\n");
                    }
                };

                ExportTo(std::string(pathBuffer) + "/" + std::string(exportNameBuffer) + ".obj", ObjExporter::GenerateFileContents(modelObjects));

                inExportGUI = false;
                hasExported = true;
            };

            ImGuiHelper::SpacedSep();
            ImGui::InputTextWithHint("##Name-Export", "name:", exportNameBuffer, IM_ARRAYSIZE(exportNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            ExportIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            ImGui::InputTextWithHint("##Path-Export", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
            ExportIf(ImGui::IsItemFocused() && input->Pressed(GLFW_KEY_ENTER));
            ExportIf(ImGui::Button("Export As .OBJ"));

            popupWarning.Update(deltaTime);
            popupWarning.RenderGUI();
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    TiledTextureAtlas openFileTextureAtlas;
    const auto OpenFileGUI = [&] () {
        MenuUpdate();

        static std::string lastPath;
        static std::vector<std::string> subFolders;
        static std::vector<std::filesystem::path> paths;

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("Open File", nullptr, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoTitleBar);
        {
            SubGuiExitOptions();

            static char pathBuffer[256] = "../output";

            if (enteringGuiScreen) {
                lastPath = "";
//                ImGui::SetKeyboardFocusHere();
            }

            ImGuiHelper::SpacedSep();
            ImGui::InputTextWithHint("##Path-Open", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
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
                for (const std::string pathStr : paths) {
                    auto& ref = metaReads.emplace_back(DeserializeMeta(pathStr));
                    const int desiredSize = 64 * 64 * 3;
                    if (ref.img.size() != desiredSize) {
                        printf("[Warning]: Preview image is incorrect size!\n");
                        ref.img.clear();
                        ref.img.reserve(desiredSize);
                        for (int i = 0; i < desiredSize; i++) {ref.img.emplace_back(0);}
                    }
                }

                openFileTextureAtlas.Generate(64, Linq::Select<Serialization, std::vector<unsigned char>*>(metaReads, [](Serialization& refVal){
                    return &(refVal.img);
                }));
            }

            bool reloadFolder = false;
            if (!pathExists) {
                ImGui::Text("Invalid path!");
            } else if (paths.empty() && subFolders.empty()) {
                ImGui::Text("Nothing here... ¯\\_(*_*)_/¯");
            } else {

                static float previewSize = 128.0f;
                static float padding = 16.0f;

                const float availWidth = ImGui::GetWindowContentRegionWidth();
                const int columnCount = std::max(1, (int) ((availWidth + padding) / (previewSize + padding)));

                ImGui::Columns(columnCount, nullptr, false);

                int childPathIndex = 0;
                for (auto& childPath : paths) {
                    const auto OnPress = [&] {
                        DeSerializeScene(childPath);
                        inOpenFileGUI = false;
                    };

                    const auto& [uv1, uv2] = openFileTextureAtlas.GetCoords(childPathIndex);
                    ImGui::ImageButton((void*)(intptr_t)openFileTextureAtlas.ID, {previewSize, previewSize}, Util::ToImVec(uv1), Util::ToImVec(uv2));
                    if (ImGui::IsItemClicked()) OnPress();
                    if (ImGui::IsItemHovered() && input->mouseRightPressed) {
                        ImGui::OpenPopup(childPath.c_str());
                    }
                    if (ImGuiHelper::HoverDelayTooltip()) ImGui::SetTooltip("%s", childPath.c_str());
                    if (ImGui::BeginPopup(childPath.c_str())) {

                        if (ImGui::Button("Delete")) {
                            std::filesystem::remove(childPath);
                            reloadFolder = true;
                        }

                        ImGui::EndPopup();
                    }
                    childPathIndex++;
                    { // displays a shortened version of the filename
                        // TODO: once filename vs. extension is serialized properly, this should be removed
                        std::string fileName = childPath.filename();
                        const int index = (int) fileName.find_last_of('.');
                        if (index != -1) {
                            fileName = fileName.substr(0, index);
                        }
                        const std::string fullFileName = fileName;
                        bool shortened = false;
                        const float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
                        static float characterWidth = ImGui::CalcTextSize(" ").x; // (monospaced font)
                        if (textWidth > previewSize) {
                            fileName = fileName.substr(0, (int)(textWidth / characterWidth) - 1) + "-";
                            shortened = true;
                        }

                        ImGui::Text("%s", fileName.c_str());
                        if (shortened && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", fullFileName.c_str());
                    }
                    ImGui::NextColumn();
                }

                ImGui::Columns(1);
            }

            for (auto& subFolder : subFolders) {
                ImGui::BulletText("%s", subFolder.c_str());
            }

            lastPath = path;
            if (reloadFolder) lastPath = "";
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    const auto ControlsGUI = [&] {
        MenuUpdate();

        if (enteringGuiScreen) ImGui::SetNextWindowFocus();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
        ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoResize |
                                           ImGuiWindowFlags_NoMove |
                                           ImGuiWindowFlags_NoCollapse |
                                           ImGuiWindowFlags_NoTitleBar);
        {
            SubGuiExitOptions();

            ImGuiHelper::SpacedSep();

            Controls::GUI();
        }
        ImGui::End();

        enteringGuiScreen = false;
    };

    while (!window.ShouldClose()) // >> UPDATE LOOP ======================================
    {
        WIDTH  = window.GetWidth();
        HEIGHT = window.GetHeight();

        if (window.HasResizeDiff()) { // reconfigure anything dependent on window size (textures / projection)
            SetupProjection();
            modelScene.ChangeDimensions(window.GetBufferWidth(), window.GetBufferHeight());
            graphScene.ChangeDimensions(window.GetBufferWidth(), window.GetBufferHeight());
            window.UnDiffResize();
        }

        if (requirePlotResize) { // TODO: use proper buffer to actual rate!!!
//            Util::PrintVec(newPlotDimens);
//            Util::PrintVec({window.GetBufferWidth(), window.GetBufferHeight()});
//            graphScene.ChangeDimensions(newPlotDimens * 2.5f); // FIXME!!!!!!!!!
//            graphScene.ChangeDimensions(window.GetBufferWidth(), window.GetBufferHeight());
//            graphScene.ChangeDimensions(window.GetBufferWidth(), window.GetBufferHeight());
            requirePlotResize = false;
        }

        // setup time
        auto time = (float) glfwGetTime();
        float deltaTime = time - lastTime;

        // GLOBAL HOTKEYS

        if (Controls::Check(CONTROLS_OpenFileSaveMenu)) {
            inSaveFileGUI = true;
            inOpenFileGUI = false;
            inControlsGUI = false;
            inExportGUI = false;
            enteringGuiScreen = true;
        }

        if (Controls::Check(CONTROLS_OpenFileOpenMenu)) {
            inOpenFileGUI = true;
            inSaveFileGUI = false;
            inControlsGUI = false;
            inExportGUI = false;
            enteringGuiScreen = true;
        }

        if (Controls::Check(CONTROLS_OpenControlsMenu)) {
            inControlsGUI = true;
            inOpenFileGUI = false;
            inSaveFileGUI = false;
            inExportGUI = false;
            enteringGuiScreen = true;
        }

        if (Controls::Check(CONTROLS_OpenExportMenu)) {
            inControlsGUI = false;
            inOpenFileGUI = false;
            inSaveFileGUI = false;
            inExportGUI = true;
            enteringGuiScreen = true;
        }

        // Input Updating
        input->EndUpdate();
        glfwPollEvents();
        Vec2 onScreen = Util::NormalizeToRectNPFlipped(input->GetMouse(), plotRect);


        // Check whether to defer program flow
        bool executionPaused = inControlsGUI || inOpenFileGUI || inSaveFileGUI || inExportGUI;

        if (executionPaused) {
            ImGuiHelper::BeginFrame();
            {
                if (inSaveFileGUI) SaveFileGUI(deltaTime);
                if (inOpenFileGUI) OpenFileGUI();
                if (inControlsGUI) ControlsGUI();
                if (inExportGUI) ExportGUI(deltaTime);
            }
            ImGuiHelper::EndFrame();
        } else {
            // hotkeys
            if (developerMode && input->Pressed(GLFW_KEY_ESCAPE)) window.Close();

            if (input->Pressed(GLFW_KEY_L)) SetCameraMode(!cameraMode);


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
            for (const auto renderModelObject: modelObjects) {
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
                            netDrag += glm::length(displayMousePos -
                                                   Util::NormalizeToRectNPFlipped(input->GetLastMouse(), displayRect));
                        }

                        if (input->mouseUnpressed && netDrag < 0.0001f) { // only select if user did not drag mouse
                            SetModelObject((ModelObject *) obj);
                        }
                    }
                }
            }

            shader.Disable();


            RenderTarget::Unbind();

            { // >> 2D PLOT RENDER ==============================
                RenderTarget::Bind(graphScene);

                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                shader2D.Enable();

                shader2D.SetModel(graphView.GenProjection());
                plot.SetLineScale(graphView.scale);

                plot.AddQuad({-1.0f, 0.0f}, {1.0f, -1.0f}, {1.0f, 1.0f, 1.0f, 1.0f});
                graphView.Render({plot, *input});

                if (!timeline.IsPlaying()) timeline.RenderOnionSkin(plot, drawMode);

                modelObject->Render2D({plot, drawMode, onScreen, graphView, editContext});

                plot.ImmediateClearingRender();

                shader2D.Disable();
                RenderTarget::Unbind();
            } // >> ==============================================

            // timeline
            timeline.Render({shader2D, drawMode, *input});

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);


            // >> END 2D ============================


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
                                   {1.0f, 0.0f}, 0);

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
                            draggedObj->UnParent();
                            draggedObj->SetPos(modelIntersection->pos);
                            const auto normal = glm::normalize(modelIntersection->normal);
                            const auto temp = glm::normalize(glm::cross(normal, glm::cross(normal, {0.0f, 1.0f, 0.0f})));
                            const auto quaDir = glm::quatLookAt(normal, {0.0f, 1.0f, 0.0f});
                            const auto eulers = glm::eulerAngles(quaDir);
                            draggedObj->SetEulers(Util::DirToEuler(normal));

                            auto* hitObj = (ModelObject*)(modelIntersection->obj);

                            // no recursive parenting!
                            if (hitObj != draggedObj && !hitObj->InParentChain(draggedObj)) {
                                hitObj->AppendChild(draggedObj);
                            }

                            draggedObj->TimelineDiffPos(timeline);
                            draggedObj->TimelineDiffEulers(timeline);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                displayRect = ImGuiHelper::ItemRectRemovePadding(0.0f, 0.0f);
                displayFocused = ImGui::IsItemActive();

                ImGuiHelper::InnerWindowBorders();
            }
            ImGui::End();

            // Settings Window
            ImGui::Begin("General");
            {
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);

                modelObject->AuxParameterUI({timeline});

                if (ImGui::CollapsingHeader("Graph View")) {
                    ImGui::SliderFloat("centerX", &graphView.centerAt.x, -10.0f, 10.0f);
                    ImGui::SliderFloat("centerY", &graphView.centerAt.y, -10.0f, 10.0f);
                    ImGui::SliderFloat("scale", &graphView.scale, 0.1f, 10.0f);
                }

                modelObject->HyperParameterUI({timeline});

                ImGuiHelper::InnerWindowBorders();
            }
            ImGui::End();

            // Toolbar
            ImGui::Begin("Toolbar");
            {

                modelObject->ModeSet("L1", Enums::DrawMode::MODE_PLOT, drawMode);
                modelObject->ModeSet("L2", Enums::DrawMode::MODE_GRAPH_Y, drawMode);
                modelObject->ModeSet("L3", Enums::DrawMode::MODE_GRAPH_Z, drawMode);
                modelObject->ModeSet("L4", Enums::DrawMode::MODE_CROSS_SECTION, drawMode);

                ImGuiHelper::InnerWindowBorders();
            }
            ImGui::End();

            // Model Instantiation Window
            ImGui::Begin("Models");
            {
                if (ImGui::Button("+##Lathe")) {
                    AddSetLathe();
                }
                ImGuiHelper::DelayControlTooltip(CONTROLS_AddLathe);

                ImGui::SameLine();
                if (ImGui::Button("+##CrossSectional")) {
                    AddSetCrossSectional();
                }
                ImGuiHelper::DelayControlTooltip(CONTROLS_AddCrossSectional);

                ImGui::SameLine();
                ImGui::Checkbox("Focus", &focusMode);

                for (ModelObject* currModelObject : modelObjects) {
                    if (!currModelObject->HasParent()) currModelObject->DraggableGUI(draggableUIInfo);
                }
                DragDropModelObject();

                ImGuiHelper::InnerWindowBorders();
            }
            ImGui::End();

            // Timeline window
            timeline.GUI({WIDTH, HEIGHT, *input});

            // Graph window (must be last??)
            ImGui::Begin("Graph Scene");
            {
                Vec2 displayDimens = Util::ToVec(ImGui::GetWindowContentRegionMax()) - Util::ToVec(ImGui::GetWindowContentRegionMin());
                static Vec2 lastDisplayDimens;
                if (displayDimens != lastDisplayDimens) {
                    requirePlotResize = true;
                    newPlotDimens = displayDimens;
                }
                lastDisplayDimens = displayDimens;
//                Vec2 displayDimens = Util::FitRatio({WIDTH / 2.0f, HEIGHT / 2.0f},
//                                                    Util::ToVec(ImGui::GetWindowContentRegionMax()) - Util::ToVec(ImGui::GetWindowContentRegionMin()) - Vec2(8.0f, 6.0f));
//                ImGui::SameLine((ImGui::GetWindowWidth()) / 2.0f - (displayDimens.x / 2.0f));
                ImGui::ImageButton((void *) (intptr_t) graphScene.GetTexture(), Util::ToImVec(displayDimens), {0.0f, 1.0f},
                                   {1.0f, 0.0f}, 0);

                plotRect = ImGuiHelper::ItemRectRemovePadding(0.0f, 0.0f);
                // TODO: IsItemActive works perfectly for mouse, but focus works better for keyboard :/
                graphFocused = ImGui::IsItemFocused();

                ImGuiHelper::InnerWindowBorders();
            }
            ImGui::End();

            ImGuiHelper::EndFrame();
            // ====================================

            // >> UPDATE MESH
            if (!cameraMode) {
                modelObject->EditMakeup({editContext, *input, drawMode, onScreen, camera, graphFocused, graphView, plotRect});
            }


            graphView.Update({*input, plotRect});

            Display3DContext::Update({*input, camera, displayRect, displayFocused});

            if (Controls::Check(CONTROLS_ClearAllLayers)) {
                Undos::Add(modelObject->GenAllLineStateUndo());
                modelObject->ClearAll();
                UpdateMesh();
            }
            else if (Controls::Check(CONTROLS_ClearCurrentLayer)) {
                Undos::Add(modelObject->GenLineStateUndo(drawMode));
                modelObject->ClearSingle(drawMode);
                UpdateMesh();
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

            if (Controls::Check(CONTROLS_AddCrossSectional)) AddSetCrossSectional();
            if (Controls::Check(CONTROLS_AddLathe)) AddSetLathe();

            if (Controls::Check(CONTROLS_SetLayerPrimary)) drawMode = Enums::MODE_PLOT;
            if (Controls::Check(CONTROLS_SetLayerSecondary)) drawMode = Enums::MODE_GRAPH_Y;
            if (Controls::Check(CONTROLS_SetLayerTertiary)) drawMode = Enums::MODE_GRAPH_Z;
            if (Controls::Check(CONTROLS_SetLayerQuaternary)) drawMode = Enums::MODE_CROSS_SECTION;

            if (editContext.CanUndo()) { // Undo-ing
                bool initUndo = Controls::Check(CONTROLS_Undo);
                bool holdUndo = Controls::Check(CONTROLS_UndoHold);
                if (initUndo || holdUndo) {
                    editContext.UseUndo(initUndo);
                    Undos::UseLast();
                }
            }

            // POST UPDATE EVENTS
            timeline.Update({*input, deltaTime, drawMode, *modelObject, modelObjects, focusMode});
            modelObject->UnDiffAll();
            editContext.Update(deltaTime);
        }

        // Global Post Events
        lastTime = time;

        window.SwapBuffers(); // Swap the screen buffers
    }

    // Manual Resource Clears
    ImGuiHelper::Destroy();
    glfwTerminate();

    return EXIT_SUCCESS;
}