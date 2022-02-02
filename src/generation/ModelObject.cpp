//
// Created by Tobiathan on 10/26/21.
//

#include "ModelObject.h"
#include "Sampler.h"
#include "../graphing/Function.h"
#include "../util/Util.h"

#include <glfw3.h>
#include <numeric>
#include "../animation/Timeline.h"
#include "../util/Controls.h"

int ModelObject::nextUniqueID = 0;

void ModelObject::Render3D(RenderInfo3D renderInfo) {
    renderInfo.shader3D.SetModel(GenModelMat());
    renderInfo.mainLight.SetColor(color);
    renderInfo.mainLight.Apply(renderInfo.shader3D);

    mesh.Render();
}

void ModelObject::Render2D(RenderInfo2D renderInfo) {
    RenderSelf2D(renderInfo);
    RenderGizmos2D(renderInfo);
}

void ModelObject::FunctionalAngleGizmo(RenderInfo2D renderInfo, const std::vector<glm::vec2>& points) {
//    Vec2 onCanvas = graphView.MousePosNPToCoords(onScreen); TODO: !!!!!

    const Vec2 onCanvas = renderInfo.graphView.MousePosNPToCoords(renderInfo.onScreen);
    Util::PrintVec(onCanvas);
    printf("eeeee");

    float pointY = Function::GetY(points, onCanvas.x);
    float pointAngle = Function::GetAverageRadians(points, onCanvas.x, 5);
    if (pointY != 0) {
        std::vector<glm::vec2> gizmo {{onCanvas.x, pointY}, glm::vec2(onCanvas.x, pointY) + Util::Polar(0.1f, -pointAngle)};
        renderInfo.plot.AddLines(gizmo, {1.0f, 0.0f, 1.0f, 1.0f});
    }
}

void ModelObject::RenderGizmos3D(RenderInfo3D renderInfo) {}

EditingContext::TransformStartInfo GenTransformStartInfo(EditingInfo info) {
    return {info.graphView.MousePosNPToCoords(info.onScreen)};
}

void ModelObject::SetPoints(Enums::DrawMode drawMode, const Vec2List& newPoints) {
    Vec2List& points = GetPointsRefByMode(drawMode);
    points.clear();
    points.insert(points.begin(), newPoints.begin(), newPoints.end());
    DiffPoints(drawMode);
    UpdateMesh();
};

void ModelObject::ReversePoints(Enums::DrawMode drawMode) {
    Vec2List& points = GetPointsRefByMode(drawMode);
    SetPoints(drawMode, Linq::Select<Vec2, Vec2>(points, [&](const Vec2& _, int i) {
        return points[points.size() - i];
    }));
}

void ModelObject::FlipPoints(Enums::DrawMode drawMode, Enums::Direction direction) {
    Vec2List& points = GetPointsRefByMode(drawMode);
    Vec2 avgPos = Util::AveragePos(points);
    if (direction == Enums::HORIZONTAL) SetPoints(drawMode, Linq::Select<Vec2, Vec2>(points, [=](const Vec2& vec) {
        return Vec2(avgPos.x - (vec.x - avgPos.x), vec.y);
    }));
    else SetPoints(drawMode, Linq::Select<Vec2, Vec2>(points, [=](const Vec2& vec) {
        return Vec2(vec.x, avgPos.y - (vec.y - avgPos.y));
    }));
}

void ModelObject::EditCurrentLines(EditingInfo info) {
    const auto& [editContext, input, drawMode, onScreen, camera, graphFocused, graphView] = info;

    Vec2 onCanvas = graphView.MousePosNPToCoords(onScreen);

    auto& points = GetPointsRefByMode(info.drawMode);

    const auto Diff = [&](const std::function <glm::vec2(glm::vec2)>& vectorFunc) {
        for (auto& point : points) {
            point = vectorFunc(point);
        }
        DiffPoints(info.drawMode);
        UpdateMesh();
    };

    const auto DiffBase = [&](const std::function <glm::vec2(glm::vec2)>& vectorFunc) {
        for (int i = 0; i < points.size(); i++) {
            points[i] = vectorFunc(info.editContext.transformStoreInitPoints[i]);
        }
        DiffPoints(info.drawMode);
        UpdateMesh();
    };

    const auto BindTransformTriggerKey = [&](int controlCode, Enums::TransformationType mode) {
        if (Controls::Check(controlCode)) info.editContext.StartTransform(mode, GenTransformStartInfo(info), points);
    };

    BindTransformTriggerKey(CONTROLS_Drag, Enums::TRANSFORM_DRAG);
    BindTransformTriggerKey(CONTROLS_Rotate, Enums::TRANSFORM_ROTATE);
    BindTransformTriggerKey(CONTROLS_Scale, Enums::TRANSFORM_SCALE);
    BindTransformTriggerKey(CONTROLS_Smear, Enums::TRANSFORM_SMEAR);

    if (Controls::Check(CONTROLS_FlipHoriz)) FlipPoints(info.drawMode, Enums::HORIZONTAL);
    if (Controls::Check(CONTROLS_FlipVert)) FlipPoints(info.drawMode, Enums::VERTICAL);
    if (Controls::Check(CONTROLS_ReversePoints)) ReversePoints(info.drawMode);

    // TODO: add eraser tool!
    if (input.Down(GLFW_KEY_E)) {
        bool diffFlag = false;
        for (int i = (int) points.size() - 1; i >= 0; i--) {
            if (glm::length(onCanvas - points[i]) < 0.05f) {
                points.erase(points.begin() + i);
                diffFlag = true;
            }
        }
        if (diffFlag) {
            DiffPoints(drawMode);
            UpdateMesh();
        }
    }

    if (editContext.IsTransformationActive()) {
        switch (editContext.GetTransformationType()) {
            case Enums::TRANSFORM_DRAG: {
                glm::vec2 delta = onCanvas - editContext.GetLastMousePos();
                Diff([=](glm::vec2 vec) {
                    return vec + delta;
                });
                break;
            }
            case Enums::TRANSFORM_ROTATE: {
                const Vec2 avgPos = Util::AveragePos(points);

                float lastAngle = Util::Angle(editContext.GetLastMousePos() - avgPos);
                float newAngle = Util::Angle(onCanvas - avgPos);
                float angleDiff = newAngle - lastAngle;

                Diff([=](Vec2 vec) {
                    float angle = Util::Angle(vec - avgPos);
                    float mag = glm::length(vec - avgPos);
                    return avgPos + Util::Polar(mag, -(angle - (float) M_PI_2 + angleDiff));
                });
                break;
            }
            case Enums::TRANSFORM_SCALE: {
                const Vec2 avgPos = Util::AveragePos(points);

                float lastMag = glm::length(editContext.GetLastMousePos() - avgPos);
                float newMag = glm::length(onCanvas - avgPos);

                Diff([=](glm::vec2 vec) {
                    float angle = Util::Angle(vec - avgPos);
                    float mag = glm::length(vec - avgPos);
                    return avgPos + Util::Polar(mag / lastMag * newMag, -(angle - (float) M_PI_2));
                });
                break;
            }
            case Enums::TRANSFORM_SMEAR: {
                const auto SmearFalloff = [](float x) {
                    return 1.0f / (1.0f + exp((x - 0.24f) * 20.0f));
                };

                glm::vec2 initPos = editContext.GetTransformStartPos();
                glm::vec2 delta = onCanvas - initPos;

                DiffBase([=](glm::vec2 vec) {
                    float initMag = glm::length(vec - initPos);
                    return vec + delta * SmearFalloff(initMag);
                });
                break;
            }
        }

        if (input.mousePressed) {
            editContext.FinalizeTransform();
            editContext.DisableDrawingForClick();
        } else if (input.Pressed(GLFW_KEY_ENTER) || input.Pressed(GLFW_KEY_SPACE)) {
            editContext.FinalizeTransform();
        } else if (input.Pressed(GLFW_KEY_LEFT_SHIFT) || input.mouseRightPressed) {
            editContext.CancelTransform(points);
            UpdateMesh();
        }

    } else {
        if (input.mouseDown && editContext.IsDrawingEnabledForClick() && info.graphFocused) {
            bool mouseOnGUI = Util::VecIsNormalizedNP(info.onScreen);
            if (mouseOnGUI) {
                InputPoints({info.drawMode, info.onScreen, graphView, info.camera});
            }
        }
    }


    // END ACTIONS
    if (input.mouseUnpressed) {
        editContext.OnMouseUp();
    }

    editContext.SetLastMousePos(onCanvas);
}

void ModelObject::EditMakeup(EditingInfo info) {
    EditCurrentLines(info);
}

void ModelObject::InputPoints(MouseInputInfo info) {
    const auto& onScreen = info.onScreen;
    auto& vec = GetPointsRefByMode(info.drawMode);

    Vec2 canvasPoint = info.graphView.MousePosNPToCoords(onScreen);

    switch (LineTypeByMode(info.drawMode)) {
        case Enums::POLYLINE:
            if (vec.empty() || vec.back() != canvasPoint) {
                vec.emplace_back(canvasPoint);
                DiffPoints(info.drawMode);
                UpdateMesh();
            }
            break;
        case Enums::PIECEWISE:
            if (vec.empty() || canvasPoint.x > vec.back().x) {
                vec.emplace_back(canvasPoint);
                DiffPoints(info.drawMode);
                UpdateMesh();
            }
            break;
    }

}

void ModelObject::ClearSingle(Enums::DrawMode drawMode) {
    GetPointsRefByMode(drawMode).clear();
    DiffPoints(drawMode);
}

void ModelObject::UnParent(bool maintainAbsoluteTransform) {
    if (!HasParent()) return;

    const auto initModelMatDescription = Util::DecomposeMatrix(GenModelMat());

    for (int i = 0; i < parent->children.size(); i++) {
        if (parent->children[i] == this) {
            parent->children.erase(parent->children.begin() + i);
            break;
        }
    }

    parent = nullptr;

    if (maintainAbsoluteTransform) {
        PersistModelMat(initModelMatDescription, {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}});
    }
}

void ModelObject::AppendChild(ModelObject *child, bool maintainAbsoluteTransform) {
    const auto initChildModelMatDescription = Util::DecomposeMatrix(child->GenModelMat());

    child->UnParent();
    child->parent = this;

    if (maintainAbsoluteTransform) {
        child->PersistModelMat(initChildModelMatDescription, Util::DecomposeMatrix(GenModelMat()));
    }

    children.push_back(child);
}

void ModelObject::DraggableGUI(const DraggableUIInfo& info) {
    const auto&[isSelected, select, addObj, deleteObj] = info;

    std::string nameID = "Elem - " + std::to_string(ID);

    ImGui::PushID(this);
    bool isOpen = ImGui::TreeNodeEx(nameID.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth |
            (children.empty() ? ImGuiTreeNodeFlags_Bullet : 0) |
            (isSelected(this) ? ImGuiTreeNodeFlags_Selected : 0));

    std::string popupID = std::string("ModelObject-") + std::to_string(ID);

    if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1]) {
        ImGui::OpenPopup(popupID.c_str());
    }

    if (ImGui::BeginPopup(popupID.c_str())) {

        ImGui::Checkbox("Visible", &visible);

        if (ImGui::Button("Copy")) {
            addObj(this->CopyRecursive());
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::Button("Delete")) {
            deleteObj(this);
        }

        ImGui::EndPopup();
    }

    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        if (!isSelected(this)) select(this);
    }

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
    {
        ModelObject* ptr = this;
        ModelObject** payload = &ptr;
        ImGui::SetDragDropPayload("ModelObjectDrag", (void*)payload, sizeof(ModelObject*));

        ImGui::Text("%s", nameID.c_str()); // drag-preview
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ModelObjectDrag"))
        {
            IM_ASSERT(payload->DataSize == sizeof(ModelObject*));
            ModelObject* draggedObj = *(ModelObject**)payload->Data;
            if (!InParentChain(draggedObj)) {
                AppendChild(draggedObj);
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (isOpen) {
        for (ModelObject* child : children) {
            child->DraggableGUI(info);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}

bool ModelObject::InParentChain(ModelObject* obj) {
    if (!HasParent()) return false;
    if (parent == obj) return true;
    return parent->InParentChain(obj);
}

ModelObject *ModelObject::CopyRecursive() {
    ModelObject* copy = CopyInternals();

#define QUICK_COPY(a) copy->a = a

    QUICK_COPY(modelTranslation);
    QUICK_COPY(color);
    QUICK_COPY(eulerAngles);
    QUICK_COPY(animator);
    QUICK_COPY(sampleLength);
    QUICK_COPY(visible);
    copy->ID = GenUniqueID();

    copy->parent = nullptr;
    copy->children = {};

    for (ModelObject* child : children) {
        copy->AppendChild(child->CopyRecursive(), false);
    }

    return copy;
}


void ModelObject::AnimatableSliderValUpdateBound(const std::string& label, float* ptr, Timeline& timeline, float min, float max) {
    bool animated = timeline.HasFloatLayer(label);
    ImGui::Checkbox(("##" + label).c_str(), &animated);
    if (ImGui::IsItemClicked()) {
        if (!animated) {
            timeline.AddFloatLayer(label, *ptr);
        } else {
            timeline.RemoveFloatLayer(label);
        }
    }

    bool hasMin = !std::isnan(min), hasMax = !std::isnan(max);

    ImGui::SameLine();
    if (hasMin && hasMax) {
        ImGui::SliderFloat(label.c_str(), ptr, min, max);
    } else {
        ImGui::DragFloat(label.c_str(), ptr, 0.025f);
    }

    if (ImGui::IsItemActive()) {
        if (hasMin) *ptr = std::max(min, *ptr);
        if (hasMax) *ptr = std::min(max, *ptr);

        if (animated) {
            timeline.UpdateFloat(label, *ptr);
        }
        UpdateMesh();
    }
}

void ModelObject::PersistModelMat(MatrixComponents initFull, MatrixComponents parentMat) {
    const auto&[childTranslate, childDir] = initFull;
    const auto&[translate, dir] = parentMat;
    const Vec3 diff = childTranslate - translate;
    const Vec3 parentEulers = Util::DirToEuler(dir);
    const glm::mat4 rot = glm::toMat4(glm::quat(parentEulers));
    const Vec3 rotatedDiff = glm::inverse(rot) * Vec4(diff, 0.0f);
    SetPos(rotatedDiff);

    SetEulers(Util::DirToEuler(glm::inverse(rot) * Vec4(childDir, 0.0f)));
}
