//
// Created by Tobiathan on 11/13/21.
//

#include "Timeline.h"
#include "../util/ImGuiHelper.h"
#include "../util/Util.h"

void Timeline::Update(const TimelineUpdateInfo& info) {
    const auto&[input, deltaTime, modelObject, drawMode] = info;

    const auto SampleAtTime = [&](float time) {
        bool diffFlag = false;

        for (auto& [mode, keyFrameLayer] : keyFrameLayers) {
            if (keyFrameLayer.HasValue()) {
                auto& pointsRef = info.modelObject.GetPointsRefByMode(mode);
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

    if (focused) {

        // INPUT =======
        if (input.Pressed(GLFW_KEY_SPACE)) playing ^= true;
        // add keyframe
        if (input.Pressed(GLFW_KEY_K)) {
            const std::vector<glm::vec2> pointsRef = modelObject.GetPointsRefByMode(drawMode);

            if (pointsRef.size() >= 2) {
                keyFrameLayers[drawMode].Insert({pointsRef, currentTime});
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

        // mouse pos ===
        glm::vec2 mousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
        bool mouseOnGUI = Util::VecIsNormalizedNP(mousePos);

        // Time-picker ===
        if (mouseOnGUI && input.mousePressed && lastFocused) {
            currentTime = std::round(mousePos.x * 10.0f) / 10.0f;
            //TODO: SampleAtTime(currentTime);
            // MUST HAVE AUTO-CREATION OF KEYFRAMES FIRST!!!
        }
    }

    if (playing) {
        currentTime += deltaTime;
        if (currentTime > 1.0f) currentTime = -1.0f;

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