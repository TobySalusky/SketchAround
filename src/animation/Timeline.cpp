//
// Created by Tobiathan on 11/13/21.
//

#include "Timeline.h"
#include "../util/ImGuiHelper.h"
#include "../util/Util.h"
#include "blending/SineBlendMode.h"
#include "blending/PiecewiseBlendMode.h"
#include "Animator.h"

std::vector<int> numKeys = {
    GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3,
    GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6,
    GLFW_KEY_7, GLFW_KEY_8, GLFW_KEY_9
};

void Timeline::Update(const TimelineUpdateInfo& info) {
    const auto&[input, deltaTime, drawMode, modelObject, modelObjects, focusMode] = info;

    const float currentTime = animator->currentTime;
    auto& keyFrameLayers = animator->keyFrameLayers;
    auto& floatKeyFrameLayers = animator->floatKeyFrameLayers;

    const auto SampleAtTime = [](ModelObject& obj, float time) {
        bool diffFlag = false;

        auto& keyFrameLayers = obj.GetAnimatorPtr()->keyFrameLayers;
        auto& floatKeyFrameLayers = obj.GetAnimatorPtr()->floatKeyFrameLayers;

        for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
            if (keyFrameLayer.HasValue()) {
                Vec2List& pointsRef = obj.GetPointsRefByMode(mode);

                auto animatedPoints = keyFrameLayer.GetAnimatedVal(time);
                pointsRef.clear();
                pointsRef.insert(pointsRef.end(), animatedPoints.begin(), animatedPoints.end());
                diffFlag = true;
            }
        }

        for (auto& [valLabel, settableKeyFrameLayer] : floatKeyFrameLayers) {
            if (settableKeyFrameLayer.layer.HasValue()) {
                obj.SetValueByLabel(valLabel, settableKeyFrameLayer.layer.GetAnimatedVal(time));
                diffFlag = true;
            }
        }

        if (diffFlag) {
            obj.UpdateMesh();
        }
    };

    const auto SampleAllAtTime = [&](float time) {
        if (info.focusMode) {
            SampleAtTime(info.modelObject, time);
            return;
        }

        for (ModelObject* obj : info.modelObjects) {
            SampleAtTime(*obj, time);
        }
    };

    // mouse pos ===
    glm::vec2 mousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
    bool mouseOnGUI = Util::VecIsNormalizedNP(mousePos);


    if (hasSelection && input.Pressed(GLFW_KEY_V)) {
        selection.DeleteAll();
        hasSelection = false;
    }

    if (hasSelection) {
        for (int i = 0; i < numKeys.size(); i++) {
            if (input.Pressed(numKeys[i])) selection.SetBlendID((int) i);
        }
    }

    // MOUSE INPUT ==============
    if (mouseOnGUI && input.mousePressed) {
        if (mousePos.y < 1.0f - selectAreaSize) {
            if (focused) {
                selecting = true;
                hasSelection = false;
                selectDragStart = mousePos;
            }
        } else {
            if (focused) {
                selecting = false;
                hasSelection = false;
            }
        }
    }

    // selecting ===
    if (input.mouseDown && focused) {
        if (selecting) {
            selectDragEnd = mousePos;
            selection = GenTimelineSelection();
        } else {
            // Time-picker ===
            if (mouseOnGUI) {
                const float prevTime = animator->currentTime;
                animator->currentTime = std::round(mousePos.x * 10.0f) / 10.0f;
                if (animator->currentTime != prevTime) SampleAllAtTime(animator->currentTime);
            }
        }
    }

    if (selecting && input.mouseUnpressed) {
        if (glm::length(selectDragEnd - selectDragStart) <= 0.001f) {
            Vec2 midpoint = selectDragStart;
            Vec2 diff = {0.05f, 0.02f};
            selectDragStart = midpoint - diff;
            selectDragEnd = midpoint + diff;
        }
        selecting = false;
        selection = GenTimelineSelection();
        hasSelection = selection.CountKeyframes() > 0;
    }
    // =============================

    if (focused) {
        if (input.Pressed(GLFW_KEY_SPACE)) playing ^= true;
    }

    if (!playing) {
        // auto-keying points
        {
            for (const auto mode : {Enums::MODE_PLOT, Enums::MODE_GRAPH_Y, Enums::MODE_GRAPH_Z, Enums::MODE_CROSS_SECTION}) {
                if (modelObject.HasDiff(mode)) {
                    if (!modelObject.GetPointsRefByMode(mode).empty()) {
                        keyFrameLayers[mode].Insert({modelObject.GetPointsRefByMode(mode), currentTime});
                    } else {
                        keyFrameLayers[mode].RemoveAtTime(currentTime);
                    }
                }
            }
        }

        if (focused) {

            // INPUT =======
            // add keyframe
            if (input.Pressed(GLFW_KEY_M)) {
                BlendModes::Add(new PiecewiseBlendMode(modelObject.GetPointsRefByMode(drawMode), BlendModes::GetNextID()));
            }

            if (input.Pressed(GLFW_KEY_K)) {
                const Vec2List pointsRef = modelObject.GetPointsRefByMode(drawMode);

                if (pointsRef.size() >= 2) {
                    KeyFrame<Vec2List> frame = {pointsRef, currentTime};
                    frame.blendModeID = 2;
                    keyFrameLayers[drawMode].Insert(frame);
                }
            }

            if (input.Pressed(GLFW_KEY_X)) {
                if (input.Down(GLFW_KEY_LEFT_SHIFT)) {
                    for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
                        keyFrameLayers[mode].RemoveAtTime(currentTime);
                    }

                    for (auto& [valLabel, keyFrameLayer] : floatKeyFrameLayers) {
                        floatKeyFrameLayers[valLabel].layer.RemoveAtTime(currentTime);
                    }
                } else {
                    keyFrameLayers[drawMode].RemoveAtTime(currentTime);
                }
            }
        }

    } else {
        animator->currentTime += deltaTime * playbackSpeed;

        if (currentTime > 1.0f) {
            if (pingPong) {
                animator->currentTime = 1.0f;
                playbackSpeed *= -1.0f;
            }
            else animator->currentTime = -1.0f;
        } else if (currentTime < -1.0f) {
            if (pingPong) {
                animator->currentTime = -1.0f;
                playbackSpeed *= -1.0f;
            }
            else animator->currentTime = 1.0f;
        }

        SampleAllAtTime(currentTime);
    }

    lastFocused = focused;
}

void Timeline::TopToBottomLineAt(float x, glm::vec4 color, float width, bool trueTop) {
    canvas.AddLines({{x, -1.0f}, {x, trueTop ? 1.0f : 1.0f - selectAreaSize}}, color, width);
}

void Timeline::Render(const TimelineRenderInfo& info) {

    const auto& [shader2D, drawMode, input] = info;

    const float currentTime = animator->currentTime;
    auto& keyFrameLayers = animator->keyFrameLayers;
    auto& floatKeyFrameLayers = animator->floatKeyFrameLayers;

    RenderTarget::Bind(scene);
    glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader2D.Enable();

    float height = RowToHeight(drawMode);
    canvas.AddQuad({-1.0f, height - 0.1f}, {1.0f, height + 0.1f}, {0.21f, 0.21f, 0.21f, 1.0f});
    canvas.AddQuad({-1.0f, 1.0f}, {1.0f, 1.0f - selectAreaSize}, {0.15f, 0.15f, 0.15f, 1.0f});

    for (float i = -0.9f; i < 1.0f; i += 0.1f) { // NOLINT(cert-flp30-c)
        TopToBottomLineAt(i, {0.35f, 0.35f, 0.35f, 1.0f});
    }
    TopToBottomLineAt(currentTime, focused ? glm::vec4(0.8f, 0.8f, 1.0f, 1.0f) : glm::vec4(0.55f, 0.55f, 0.7f, 1.0f), 0.004f, true);

    const auto ContainsFunc = [&](auto* ptr){
        return selection.ContainsKeyframe(ptr);
    };

    const auto NoSelectContainsFunc = [](auto* ptr) { return false; };

    for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
        if (hasSelection || selecting) keyFrameLayer.Render(canvas, (int) mode, currentTime, ContainsFunc);
        else keyFrameLayer.Render(canvas, (int) mode, currentTime, NoSelectContainsFunc);
    }

    int floatLayerInc = 0;
    for (auto& [label, keyFrameLayer] : floatKeyFrameLayers) {
        if (hasSelection || selecting) keyFrameLayer.layer.Render(canvas, 4 + floatLayerInc, currentTime, ContainsFunc);
        else keyFrameLayer.layer.Render(canvas, 4 + floatLayerInc, currentTime, NoSelectContainsFunc);
        floatLayerInc++;
    }

    if (selecting) {
        canvas.AddQuad(selectDragStart, selectDragEnd, Util::RGBA(200, 200, 255, 40));
    }

    glm::vec2 mousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
    bool mouseOnGUI = Util::VecIsNormalizedNP(mousePos);

    if (mouseOnGUI && (!input.mouseDown || selecting)) { // TODO:
        canvas.AddLines({{mousePos.x, 1.0f - selectAreaSize}, {mousePos.x, 1.0f}}, RGBA(0.8f, 0.8f, 1.0f, 0.3f), 0.003f);
    }

    canvas.ImmediateClearingRender();
    shader2D.Disable();
    RenderTarget::Unbind();
}

void Timeline::GUI(const TimelineGUIInfo& info) {
    const auto& [_0, _1, input] = info;
    //ImGui::ShowDemoWindow();

    bool mouseOnGUI = Util::VecIsNormalizedNP(Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect));

    ImGui::Begin("Timeline");
    {
        if (ImGui::Button(playing ? "||" : "|>")) {
            playing ^= true;
        }
        ImGui::SameLine();
        ImGui::Checkbox("ping-pong", &pingPong);
        ImGui::SameLine();
        ImGui::SliderFloat("playback-speed", &playbackSpeed, -5.0f, 5.0f);


        const auto dimens = Util::ToImVec(Util::ToVec(ImGui::GetContentRegionAvail()) - Vec2(8.0f, 6.0f));
        ImGui::ImageButton((void *) (intptr_t) scene.GetTexture(), dimens, {0.0f, 1.0f}, {1.0f, 0.0f});
        guiRect = ImGuiHelper::ItemRectRemovePadding(4.0f, 3.0f);
        focused = ImGui::IsItemFocused();

        if (hasSelection && (input.Pressed(GLFW_KEY_J) || (input.mouseRightPressed && mouseOnGUI))) {
            ImGui::OpenPopup("blendmode_dropdown");
        }

        if (ImGui::BeginPopup("blendmode_dropdown"))
        {
            ImGui::Text("Blend-mode");
            ImGui::Separator();

            const auto blendIDs = BlendModes::GenAllIDs();
            int i = 0;
            for (int blendID: blendIDs) {
                if (ImGui::Button((std::to_string(blendID + 1) + " - " + BlendModes::Get(blendID)->GetName() + "##blend_mode").c_str())) {
                    selection.SetBlendID(blendID);
                    ImGui::CloseCurrentPopup();
                }
            }

            if (ImGui::Button("New Custom")) {
                // TODO:
                ImGui::CloseCurrentPopup();
            }

            ImGui::Separator();
            if (ImGui::Button("Delete")) {
                hasSelection = false;
                selection.DeleteAll();
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void Timeline::RenderOnionSkin(Mesh2D& plot) {

    const float currentTime = animator->currentTime;
    auto& keyFrameLayers = animator->keyFrameLayers;

    const auto back = keyFrameLayers[Enums::MODE_PLOT].KeyFrameBelow(currentTime);
    const auto forward = keyFrameLayers[Enums::MODE_PLOT].KeyFrameAbove(currentTime);

    if (back.has_value()) plot.AddLines(back.value()->val, Util::RGB(255, 185, 185));
    if (forward.has_value()) plot.AddLines(forward.value()->val, Util::RGB(197, 255, 175));
}

TimelineSelection Timeline::GenTimelineSelection() {
    auto& keyFrameLayers = animator->keyFrameLayers;
    auto& floatKeyFrameLayers = animator->floatKeyFrameLayers;

    TimelineSelection newSelection;

    const float minTime = std::min(selectDragStart.x, selectDragEnd.x);
    const float maxTime = std::max(selectDragStart.x, selectDragEnd.x);
    const float minPosY = 1.0f - std::max(selectDragStart.y, selectDragEnd.y) - selectAreaSize;
    const float maxPosY = 1.0f - std::min(selectDragStart.y, selectDragEnd.y) - selectAreaSize;
    const int minIndex = (int) ((minPosY + 0.05f) / 0.2f);
    const int topIndex = (int) ((maxPosY - 0.05f) / 0.2f);

    // TODO: check for y range, not just time

    const auto InRange = [=](const float time, const int index) {
        return time >= minTime && time <= maxTime && index >= minIndex && index <= topIndex;
    };

    int i = 0;

    // TODO: generalize
    for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
        std::vector<KeyFrame<Vec2List>*> keyFramePtrs;
        for (KeyFrame<Vec2List>& val : keyFrameLayer.frames) {
            if (InRange(val.time, 3 - i)) {
                keyFramePtrs.emplace_back(&val);
            }
        }

        if (!keyFramePtrs.empty()) {
            newSelection.rowSelections.emplace_back(KeyFrameRowSelection<Vec2List>{&keyFrameLayer, keyFramePtrs, i});
        }
        i++;
    }

    for (auto& [valLabel, keyFrameLayer] : floatKeyFrameLayers) {
        std::vector<KeyFrame<float>*> keyFramePtrs;
        for (KeyFrame<float>& val : keyFrameLayer.layer.frames) {
            if (InRange(val.time, i)) {
                keyFramePtrs.emplace_back(&val);
            }
        }

        if (!keyFramePtrs.empty()) {
            newSelection.rowSelections.emplace_back(KeyFrameRowSelection<float>{&keyFrameLayer.layer, keyFramePtrs, i});
        }
        i++;
    }

    return newSelection;
}



