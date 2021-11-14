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
#include <unordered_map>

class Timeline {
public:
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

private:

    bool playing = false;
    bool focused = false, lastFocused = false;
    float currentTime = 0.0f;
    Mesh2D canvas;
    RenderTarget scene;
    Rectangle guiRect;

    std::unordered_map<Enums::DrawMode, KeyFrameLayer<std::vector<glm::vec2>>> keyFrameLayers;

    explicit Timeline(RenderTarget timelineScene) : scene(timelineScene) {
        keyFrameLayers[Enums::MODE_PLOT] = KeyFrameLayer<std::vector<glm::vec2>>();
        keyFrameLayers[Enums::MODE_GRAPH_Y] = KeyFrameLayer<std::vector<glm::vec2>>();
        keyFrameLayers[Enums::MODE_GRAPH_Z] = KeyFrameLayer<std::vector<glm::vec2>>();
    }

    void TopToBottomLineAt(float x, glm::vec4 color, float width = 0.001f);
};


#endif //SENIORRESEARCH_TIMELINE_H
