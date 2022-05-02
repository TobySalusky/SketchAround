//
// Created by Tobiathan on 4/13/22.
//

#include "ParamEditor.h"
#include "../program/Program.h"
#include "../screens/MainScreen.h"

void ParamEditor::Gui() { // FIXME: causing crash SIGSEGV
	// Settings Window
	ImGuiHelper::BeginComponentWindow("General");

	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);

	const auto& modelObject = Program::GetProject().GetCurrentModelObject();
	Timeline& timeline = MainScreen::GetComponents().timeline;

	modelObject->AuxParameterUI({timeline});
	modelObject->HyperParameterUI({timeline});

	ImGuiHelper::InnerWindowBorders();

	ImGui::End();
}
