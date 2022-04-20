//
// Created by Tobiathan on 11/12/21.
//

#ifndef SENIORRESEARCH_EDITINGCONTEXT_H
#define SENIORRESEARCH_EDITINGCONTEXT_H


#include "../util/Includes.h"
#include "../gl/Mesh2D.h"
#include "../misc/Undos.h"
#include "../graphing/GraphView.h"

class EditingContext {
public:

    std::vector<glm::vec2> transformStoreInitPoints;

    struct TransformStartInfo {
        glm::vec2 initMousePos;
    };

    void StartTransform(Enums::TransformationType transformType, TransformStartInfo initInfo, const std::vector<glm::vec2>& points) {
        transformationType = transformType;
        transformationActive = true;
        transformStartInfo = initInfo;
        lastMousePos = initInfo.initMousePos;

        transformStoreInitPoints = points;

        axisLock = Enums::LOCK_NONE;

        localTransform = false;
    }

    [[nodiscard]] bool IsTransformationActive() const { return transformationActive; }

    [[nodiscard]] Enums::TransformationType GetTransformationType() const { return transformationType; }

    [[nodiscard]] glm::vec2 GetTransformStartPos() const { return transformStartInfo.initMousePos; }


    void CancelTransform(std::vector<glm::vec2>& points) {
        Undos::PopLast();
        points.clear();
        points.insert(points.end(), transformStoreInitPoints.begin(),  transformStoreInitPoints.end());
        EndTransform();
    }

    void FinalizeTransform() {
        EndTransform();
    }

    void SetLastMousePos(glm::vec2 pos) {
        lastMousePos = pos;
    }

    [[nodiscard]] glm::vec2 GetLastMousePos() const {
        return lastMousePos;
    }

    [[nodiscard]] bool IsDrawingEnabledForClick() const { return !drawingDisabledForClick; }

    void DisableDrawingForClick() {
        drawingDisabledForClick = true;
    }

    void OnMouseUp() {
        drawingDisabledForClick = false;
    }

    void LockAxis(Enums::TransformAxisLock lockType) { axisLock = lockType; }

    void SetPrimaryGizmoPoint(const Vec2& _primaryGizmoPoint) { primaryGizmoPoint = _primaryGizmoPoint; }
    void SetSecondaryGizmoPoint(const Vec2& _secondaryGizmoPoint) { secondaryGizmoPoint = _secondaryGizmoPoint; }

    [[nodiscard]] Enums::TransformAxisLock GetAxisLock() const {
        return axisLock;
    }

    [[nodiscard]] Vec2 GenLockMult() const {
        switch (GetAxisLock()) {
            case Enums::LOCK_NONE:
                return {1.0f, 1.0f};
            case Enums::LOCK_X:
                return {1.0f, 0.0f};
            case Enums::LOCK_Y:
                return {0.0f, 1.0f};
        }
    }

    void RenderGizmos(Mesh2D& plot, GraphView& graphView);

    [[nodiscard]] bool IsLocalTransform() const {
        return localTransform;
    }

    void MakeLocal() { localTransform = true; }

    [[nodiscard]] bool IsDrawing() const { return isDrawing; }

    void BeginDrawing(Vec2 _drawingBeginCoords) {
    	isDrawing = true;
	    drawingBeginCoords = _drawingBeginCoords;
    }

    void EndDrawing() { isDrawing = false; }

    [[nodiscard]] bool CanUndo() const {
        return !IsDrawing() && !IsTransformationActive();
    }

    [[nodiscard]] bool CanUndoHold() const { return undoTimer <= 0.0f; }

    void UseUndo(bool initUndo) {
        undoTimer = initUndo ? undoTimerInitMax : undoTimerRegMax;
    }

    void Update(float deltaTime) {
        undoTimer -= deltaTime;
    }

    void SetSnapGrid(SnapGrid _snapGrid) { snapGrid = _snapGrid; }

    Vec2 ApplySnapGridIfAny(Vec2 actualCanvasPos);

    [[nodiscard]] SnapGrid GetSnapGrid() const { return snapGrid; }

    void BeginMovingSnapGridPoint() { movingSnapGridPoint = true; }
    void SetCurrentSnapGridPoint(Vec2 coords) { snapGridCentralPoint = coords; }
    void FinishMovingSnapGridPoint(Vec2 coords) {
    	movingSnapGridPoint = false;
    	snapGridCentralPoint = coords;
    }

	[[nodiscard]] bool IsMovingSnapGridPoint() const { return movingSnapGridPoint; }

private:
    static constexpr float undoTimerInitMax = 0.3f, undoTimerRegMax = 0.09f;
    float undoTimer = 0.0f;
    bool isDrawing;
    Vec2 drawingBeginCoords;

    bool localTransform;
    Enums::TransformAxisLock axisLock;
    bool drawingDisabledForClick = false;
    bool transformationActive = false;
    Enums::TransformationType transformationType;
    TransformStartInfo transformStartInfo;
    Vec2 lastMousePos;

    SnapGrid snapGrid = SnapGrid::None;
    Vec2 snapGridCentralPoint = {0.0f, 0.0f};
    bool movingSnapGridPoint = false;

    Vec2 primaryGizmoPoint, secondaryGizmoPoint;

    void EndTransform() {
        transformationActive = false;
        transformStoreInitPoints.clear();
    }
};


#endif //SENIORRESEARCH_EDITINGCONTEXT_H
