//
// Created by Tobiathan on 4/11/22.
//

#include "SceneView3D.h"
#include "../gl/MeshUtil.h"
#include "../program/Program.h"
#include "../screens/MainScreen.h"
#include "../gl/Display3DContext.h"

SceneView3D::SceneView3D(const GLWindow &window) :
	modelScene(window.GetBufferWidth(), window.GetBufferHeight(), true),
	projection(glm::perspective(45.0f, (GLfloat)window.GetBufferWidth()/(GLfloat)window.GetBufferHeight(), Z_NEAR, Z_FAR)) {}

void SceneView3D::Render(Project &project) {
	const Input& input = Program::GetInput();

	RenderTarget::Bind(modelScene);

	// Clear Background
	const float backgroundLevel3D = 0.1f;
	glClearColor(backgroundLevel3D, backgroundLevel3D, backgroundLevel3D, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// >> 3D RENDERING =================

	// SHADER
	shader3D.Enable();

	// UNIFORMS
	material.Apply(shader3D);
	shader3D.SetView(camera.CalculateViewMat());
	shader3D.SetProjection(projection);
	shader3D.SetCameraPos(camera.GetPos());

	// Rendering models
	const bool focusMode = MainScreen::GetComponents().sceneHierarchy.FocusModeActive();
	for (const auto& renderModelObject : project.GetModelObjects()) {
		if (!renderModelObject->IsVisible() || (focusMode && renderModelObject != project.GetCurrentModelObject())) continue;

		renderModelObject->Render3D({shader3D, mainLight});
		if (renderModelObject == project.GetCurrentModelObject()) renderModelObject->RenderGizmos3D({shader3D, mainLight});
	}
	shader3D.SetModel(glm::mat4(1.0f)); // Reset Model Mat

	// 3D Interactivity / Gizmos

	{ // TODO: make <func>
		const Vec2 displayMousePos = Util::NormalizeToRectNPFlipped(input.GetMouse(), displayRect);
		if (Util::VecIsNormalizedNP(displayMousePos)) {
			const auto modelIntersection = Intersector::MouseModelsIntersectionAtMousePos(Util::NormalizeToRectNP(input.GetMouse(), displayRect), project, camera);
			if (modelIntersection) {
				// 3D normal highlighter
				auto &[pos, norm, obj] = *modelIntersection;

				line3DGizmoLight.Apply(shader3D);
				planeGizmo.Set(MeshUtil::PolyLine({pos, pos + glm::normalize(norm) * 0.2f}));
				planeGizmo.Render();

				// 3D dragging
				static float netDrag = 0.0f;
				if (input.mousePressed) {
					netDrag = 0.0f;
				} else if (input.mouseDown) {
					netDrag += glm::length(displayMousePos -
					                       Util::NormalizeToRectNPFlipped(input.GetLastMouse(), displayRect));
				}

				// 3D selection
				if (input.mouseUnpressed && netDrag < 0.0001f) { // only select if user did not drag mouse
					SceneHierarchy::SetActiveModelObject((ModelObject *) obj);
				}
			}
		}
	}

	shader3D.Disable();


	RenderTarget::Unbind();
}

void SceneView3D::Gui(const Project &project) {
	ImGuiHelper::BeginComponentWindow("Model Scene");
	{

		Vec2 displayDimens = Util::FitRatio({Program::GetWindowWidth() / 2.0f, Program::GetWindowHeight() / 2.0f},
		                                    Util::ToVec(ImGui::GetWindowContentRegionMax()) - Util::ToVec(ImGui::GetWindowContentRegionMin()) - Vec2(8.0f, 6.0f));
		ImGui::SameLine((ImGui::GetWindowWidth()) / 2.0f - (displayDimens.x / 2.0f));

		ImGui::ImageButton(modelScene.GetTexture(),
		                   Util::ToImVec(displayDimens),
		                   {0.0f, 1.0f},
		                   {1.0f, 0.0f}, 0);

		if (ImGui::BeginDragDropTarget())
		{
			const auto modelIntersection = Intersector::MouseModelsIntersectionAtMousePos(
					Util::NormalizeToRectNP(Program::GetInput().GetMouse(), displayRect), project, camera);

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

					Timeline& timeline = MainScreen::GetComponents().timeline;
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
}

void SceneView3D::Update(Project &project, float deltaTime) {
	camera.Update(deltaTime, Program::GetInput());
}

void SceneView3D::PostUpdate(Project &project, float deltaTime) {
	Display3DContext::Update({Program::GetInput(), camera, displayRect, displayFocused});
}

std::vector<unsigned char> SceneView3D::GenPreviewSnapshot() {
	return RenderTarget::SampleCentralSquare(modelScene, 64);
}
