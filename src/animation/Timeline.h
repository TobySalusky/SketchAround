//
// Created by Tobiathan on 11/13/21.
//

#ifndef SENIORRESEARCH_TIMELINE_H
#define SENIORRESEARCH_TIMELINE_H


#include "../gl/Mesh2D.h"
#include "../gl/RenderTarget.h"
#include "../gl/GLWindow.h"
#include "../gl/shaders/Shader2D.h"
#include "../util/Rectangle.h"
#include "../generation/ModelObject.h"
#include "KeyFrameLayer.h"
#include "Animator.h"
#include <unordered_map>

class Timeline {
public:
    float playbackSpeed = 1.0f;

    struct TimelineUpdateInfo {
        const Input& input;
        float deltaTime;
        ModelObject& modelObject;
        Enums::DrawMode drawMode;
    };

    static Timeline Create(const GLWindow& window) { return Timeline({window.GetBufferWidth(), window.GetBufferHeight()}); }
    void Update(const TimelineUpdateInfo& info);
    void Render(const Shader2D& shader2D, Enums::DrawMode drawMode);

    void GUI(unsigned int WIDTH, unsigned int HEIGHT);

    [[nodiscard]] bool IsFocused() const { return focused; }
    [[nodiscard]] bool IsPlaying() const { return playing; }

    bool HasFloatLayer(const std::string& valLabel) {
        return animator->floatKeyFrameLayers.contains(valLabel);
    }

    void AddFloatLayer(const std::string& valLabel, float* valPtr) {
        animator->floatKeyFrameLayers[valLabel] = {KeyFrameLayer<float>(), valPtr};
        UpdateFloat(valLabel, *valPtr);
    }

    void RemoveFloatLayer(const std::string& valLabel) {
        animator->floatKeyFrameLayers.erase(valLabel);
    }


    void UpdateFloat(const std::string& valLabel, float val) {
        animator->floatKeyFrameLayers[valLabel].layer.Insert({val, animator->currentTime});
    }

    void RenderOnionSkin(Mesh2D& plot);

    static float RowToHeight(int row) {
        return 0.9f - (float) row * 0.2f;
    }

    void SetActiveAnimator(Animator* animator) { this->animator = animator; }

    void OnActiveModelObjectChange() {
        playing = false;
    }

private:
    bool playing = false;
    bool focused = false, lastFocused = false;
    Mesh2D canvas;
    RenderTarget scene;
    Rectangle guiRect;
    Animator* animator;

    explicit Timeline(RenderTarget timelineScene) : scene(timelineScene) {}

    void TopToBottomLineAt(float x, glm::vec4 color, float width = 0.001f);
};


#endif //SENIORRESEARCH_TIMELINE_H
