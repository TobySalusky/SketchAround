//
// Created by Tobiathan on 11/13/21.
//

#include "Timeline.h"
#include "../util/ImGuiHelper.h"
#include "../util/Util.h"
#include "blending/SineBlendMode.h"
#include "blending/PiecewiseBlendMode.h"

void Timeline::Update(const TimelineUpdateInfo& info) {
    const auto&[input, deltaTime, modelObject, drawMode] = info;

    const auto SampleAtTime = [&](float time) {
        bool diffFlag = false;

        for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
            if (keyFrameLayer.HasValue()) {
                Vec2List& pointsRef = info.modelObject.GetPointsRefByMode(mode);
                pointsRef.clear();

                auto animatedPoints = keyFrameLayer.GetAnimatedVal(time);
                pointsRef.insert(pointsRef.end(), animatedPoints.begin(), animatedPoints.end());
                diffFlag = true;
            }
        }

        for (auto& [valLabel, keyFrameLayer] : floatKeyFrameLayers) {
            if (keyFrameLayer.HasValue()) {
                *floatKeyFrameLayerPtrs[valLabel] = keyFrameLayer.GetAnimatedVal(time);
                diffFlag = true;
            }
        }

        if (diffFlag) {
            info.modelObject.UpdateMesh();
        }
    };

    // mouse pos ===
    glm::vec2 mousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
    bool mouseOnGUI = Util::VecIsNormalizedNP(mousePos);

    // Time-picker ===
    if (mouseOnGUI && input.mousePressed) {
        currentTime = std::round(mousePos.x * 10.0f) / 10.0f;
        SampleAtTime(currentTime);
    }

    if (focused) {
        if (input.Pressed(GLFW_KEY_SPACE)) playing ^= true;
    }

    if (!playing) {
        // auto-keying points
        {
            for (const auto mode : {Enums::MODE_PLOT, Enums::MODE_GRAPH_Y, Enums::MODE_GRAPH_Z}) {
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
                BlendModes::Add(new PiecewiseBlendMode(modelObject.GetPointsRefByMode(drawMode)));
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
                        floatKeyFrameLayers[valLabel].RemoveAtTime(currentTime);
                    }
                } else {
                    keyFrameLayers[drawMode].RemoveAtTime(currentTime);
                }
            }
        }

    } else {
        currentTime += deltaTime * playbackSpeed;
        if (currentTime > 1.0f) currentTime = -1.0f;
        if (currentTime < -1.0f) currentTime = 1.0f;

        SampleAtTime(currentTime);
    }

    lastFocused = focused;
}

void Timeline::TopToBottomLineAt(float x, glm::vec4 color, float width) {
    canvas.AddLines({{x, -1.0f}, {x, 1.0f}}, color, width);
}

void Timeline::Render(const Shader2D& shader2D, Enums::DrawMode drawMode) {
    RenderTarget::Bind(scene);
    glClearColor(0.17f, 0.17f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    shader2D.Enable();

    float height = RowToHeight(drawMode);
    canvas.AddQuad({-1.0f, height - 0.1f}, {1.0f, height + 0.1f}, {0.21f, 0.21f, 0.21f, 1.0f});

    for (float i = -0.9f; i < 1.0f; i += 0.1f) { // NOLINT(cert-flp30-c)
        TopToBottomLineAt(i, {0.35f, 0.35f, 0.35f, 1.0f});
    }
    TopToBottomLineAt(currentTime, focused ? glm::vec4(0.8f, 0.8f, 1.0f, 0.0f) : glm::vec4(0.55f, 0.55f, 0.7f, 0.0f), 0.004f);

    for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
        keyFrameLayer.Render(canvas, (int) mode, currentTime);
    }

    int floatLayerInc = 0;
    for (auto& [label, keyFrameLayer] : floatKeyFrameLayers) {
        keyFrameLayer.Render(canvas, 3 + floatLayerInc, currentTime);
        floatLayerInc++;
    }

    canvas.ImmediateClearingRender();
    shader2D.Disable();
    RenderTarget::Unbind();
}

void Timeline::GUI(unsigned int WIDTH, unsigned int HEIGHT) {
    ImGui::Begin("Timeline");
    {
        ImGui::ImageButton((void *) (intptr_t) scene.GetTexture(), {WIDTH / 2.0f, HEIGHT / 2.0f}, {0.0f, 1.0f}, {1.0f, 0.0f});
        guiRect = ImGuiHelper::ItemRectRemovePadding(4.0f, 3.0f);
        focused = ImGui::IsItemFocused();
    }
    ImGui::End();
}

void Timeline::RenderOnionSkin(Mesh2D& plot) {

    const auto back = keyFrameLayers[Enums::MODE_PLOT].KeyFrameBelow(currentTime);
    const auto forward = keyFrameLayers[Enums::MODE_PLOT].KeyFrameAbove(currentTime);

    if (back.has_value()) plot.AddLines(back.value()->val, Util::RGB(255, 185, 185));
    if (forward.has_value()) plot.AddLines(forward.value()->val, Util::RGB(197, 255, 175));

}



