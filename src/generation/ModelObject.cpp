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
#include "../misc/Undos.h"
#include "../misc/GuiStyle.h"

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

    const Vec2 onCanvas = renderInfo.graphView.MousePosNPToCoords(renderInfo.onScreen);

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
    Undos::Add(GenLineStateUndo(drawMode));

    Vec2List& points = GetPointsRefByMode(drawMode);
    SetPoints(drawMode, Linq::Select<Vec2, Vec2>(points, [&](const Vec2& _, int i) {
        return points[points.size() - 1 - i];
    }));
}

void ModelObject::FlipPoints(Enums::DrawMode drawMode, Enums::Direction direction) {
    Undos::Add(GenLineStateUndo(drawMode));

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
    const auto& [editContext, input, drawMode, onScreen, camera, graphFocused, graphView, guiRect] = info;

    Vec2 normOnRect = Util::NormalizeToRectNPFlipped(input.GetMouse(), guiRect);
    bool mouseIsOnRect = Util::VecIsNormalizedNP(normOnRect);
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
        if (Controls::Check(controlCode)) {
            Undos::Add(GenLineStateUndo(info.drawMode));
            info.editContext.StartTransform(mode, GenTransformStartInfo(info), points);
        }
    };

    BindTransformTriggerKey(CONTROLS_Drag, Enums::TRANSFORM_DRAG);
    BindTransformTriggerKey(CONTROLS_Rotate, Enums::TRANSFORM_ROTATE);
    BindTransformTriggerKey(CONTROLS_Scale, Enums::TRANSFORM_SCALE);
    BindTransformTriggerKey(CONTROLS_ScaleLocal, Enums::TRANSFORM_SCALE);
    {
        if (Controls::Check(CONTROLS_ScaleLocal)) editContext.MakeLocal();
    }

    BindTransformTriggerKey(CONTROLS_Smear, Enums::TRANSFORM_SMEAR);

    if (Controls::Check(CONTROLS_LockX)) editContext.LockAxis(Enums::LOCK_X);
    if (Controls::Check(CONTROLS_LockY)) editContext.LockAxis(Enums::LOCK_Y);

    if (Controls::Check(CONTROLS_FlipHoriz)) FlipPoints(info.drawMode, Enums::HORIZONTAL);
    if (Controls::Check(CONTROLS_FlipVert)) FlipPoints(info.drawMode, Enums::VERTICAL);
    if (Controls::Check(CONTROLS_ReversePoints)) ReversePoints(info.drawMode);

    // TODO: add eraser tool!
    if (Controls::Check(CONTROLS_Erase)) {
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
                const Vec2 initPoint = editContext.GetTransformStartPos();
                editContext.SetPrimaryGizmoPoint(initPoint);

                const Vec2 lockMult = editContext.GenLockMult();

                const Vec2 delta = (onCanvas - initPoint) * lockMult;
                editContext.SetSecondaryGizmoPoint(initPoint + delta);

                DiffBase([=](glm::vec2 vec) {
                    return vec + delta;
                });
                break;
            }
            case Enums::TRANSFORM_ROTATE: {

                const Vec2 avgPos = Util::AveragePos(editContext.transformStoreInitPoints);
                editContext.SetPrimaryGizmoPoint(avgPos);

                float initAngle = Util::Angle(editContext.GetTransformStartPos() - avgPos);
                float newAngle = Util::Angle(onCanvas - avgPos);
                float angleDiff = newAngle - initAngle;
                editContext.SetSecondaryGizmoPoint(onCanvas);

                DiffBase([=](Vec2 vec) {
                    float angle = Util::Angle(vec - avgPos);
                    float mag = glm::length(vec - avgPos);
                    return avgPos + Util::Polar(mag, -(angle - (float) M_PI_2 + angleDiff));
                });
                break;
            }
            case Enums::TRANSFORM_SCALE: {
                Vec2 avgPos = Util::AveragePos(editContext.transformStoreInitPoints);
                if (GetType() == Enums::LATHE && !editContext.IsLocalTransform()) avgPos.y = 0;

                editContext.SetPrimaryGizmoPoint(avgPos);
                const Vec2 usePoint = avgPos + (onCanvas - avgPos) * editContext.GenLockMult();
                editContext.SetSecondaryGizmoPoint(usePoint);

                float initMag = glm::length((editContext.GetTransformStartPos() - avgPos) * editContext.GenLockMult());
                float newMag = glm::length(usePoint - avgPos);

                DiffBase([=](Vec2 vec) {
                    float angle = Util::Angle(vec - avgPos);
                    float mag = glm::length(vec - avgPos);
                    const Vec2 delta = (avgPos + Util::Polar(mag / initMag * newMag, -(angle - (float) M_PI_2)) - vec) * info.editContext.GenLockMult();
                    return vec + delta;
                });
                break;
            }
            case Enums::TRANSFORM_SMEAR: {
                const auto SmearFalloff = [](float x) {
                    return 1.0f / (1.0f + exp((x - 0.24f) * 20.0f));
                };

                glm::vec2 initPos = editContext.GetTransformStartPos();
                glm::vec2 delta = (onCanvas - initPos) * editContext.GenLockMult();

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

    }

    if (input.mousePressed) { // draw start undos
        if (mouseIsOnRect) {
            if (editContext.IsDrawingEnabledForClick()) {
                Undos::Add(GenLineStateUndo(drawMode));
                editContext.SetIsDrawing(true);
            }
        } else {
            editContext.DisableDrawingForClick();
        }
    } else if (input.mouseUnpressed) {
        editContext.SetIsDrawing(false);
    }

    if (!editContext.IsTransformationActive()) {

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

        ImGui::Separator();

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



void ModelObject::AnimatableSliderValUpdateBound(const std::string& label, float* ptr, Timeline& timeline, float min, float max, float vSpeed) {
    bool animated = timeline.HasFloatLayer(label);
    ImGui::Checkbox(label.c_str(), &animated);
    if (ImGui::IsItemClicked()) {
        if (!animated) {
            timeline.AddFloatLayer(label, *ptr);
        } else {
            timeline.RemoveFloatLayer(label);
        }
    }

    bool hasMin = !std::isnan(min), hasMax = !std::isnan(max);

    ImGui::SameLine();
    ImGuiHelper::ValDrag(label.c_str(), ptr, vSpeed);

    if (ImGui::IsItemActive()) {
        if (hasMin) *ptr = std::max(min, *ptr);
        if (hasMax) *ptr = std::min(max, *ptr);

        if (animated) {
            timeline.UpdateFloat(label, *ptr);
        }
        UpdateMesh();
    }

    if (animated) {

        GuiStyle style {{
            {ImGuiCol_Button, COLOR_INVIS},
            {ImGuiCol_ButtonHovered, COLOR_INVIS_HOVER_HIGHLIGHT},
            {ImGuiCol_ButtonActive, COLOR_INVIS_ACTIVE_HIGHLIGHT},
            {ImGuiCol_Text, COLOR_ANIMATE_KEYFRAME_JUMPER_TEXT},
        }};

        constexpr float margin = 8.0f;
        ImGui::SameLine();
        if (ImGui::Button(("<##" + label).c_str())) {

        }
        
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - margin);
        if (ImGui::Button(("O##" + label).c_str())) {

        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - margin);
        if (ImGui::Button((">##" + label).c_str())) {

        }
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

void ModelObject::RenderTransformationGizmos(RenderInfo2D info) {
    info.editContext.RenderTransformGizmos(info.plot);
}

void ModelObject::TimelineDiffPos(Timeline& timeline) {
    timeline.TryUpdateFloat("x", modelTranslation.x);
    timeline.TryUpdateFloat("y", modelTranslation.y);
    timeline.TryUpdateFloat("z", modelTranslation.z);
}

void ModelObject::TimelineDiffEulers(Timeline& timeline) {
    timeline.TryUpdateFloat("rot-x", eulerAngles.x);
    timeline.TryUpdateFloat("rot-y", eulerAngles.y);
    timeline.TryUpdateFloat("rot-z", eulerAngles.z);
}
