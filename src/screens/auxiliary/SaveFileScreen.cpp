//
// Created by Tobiathan on 4/14/22.
//

#include "SaveFileScreen.h"
#include "../../program/Program.h"

#include <iostream>

#include <vector>
#include <boost/archive/text_oarchive.hpp>

void SaveFileScreen::Gui() {

	ImGuiHelper::BeginOutsideGuiScreen("Save File");
	ImGuiHelper::GuiScreenExitBar();

	if (Program::JustEnteredGuiScreen()) {
		memset(&nameBuffer[0], 0, sizeof(nameBuffer));
		ImGui::SetKeyboardFocusHere();
	}


	bool enterPressed = Program::GetInput().Pressed(GLFW_KEY_ENTER);

	ImGuiHelper::SpacedSep();
	ImGui::InputTextWithHint("##Name-Save", "name:", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
	bool saveTriggered = ImGui::IsItemFocused() && enterPressed; // save queued via enter in typing bar

	ImGui::InputTextWithHint("##Path-Save", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

	saveTriggered = saveTriggered || ImGui::IsItemFocused() && enterPressed; // save queued via enter in typing bar

	saveTriggered = ImGui::Button("Save...") || saveTriggered; // save button pressed


	if (saveTriggered) {
		TrySave();
	}

	popupWarning.RenderGUI();

	ImGui::End();
}

void SaveFileScreen::Update(float deltaTime) {
	popupWarning.Update(deltaTime);
}

void SaveFileScreen::TrySave() {

	if (std::string(nameBuffer).empty()) {
		popupWarning.Open("Must set file name!");
		return;
	}
	if (std::string(pathBuffer).empty()) {
		popupWarning.Open("Must set file path!");
		return;
	}

	SerializeScene(std::string(pathBuffer) + "/" + std::string(nameBuffer) + ".mdl", std::string(nameBuffer));

	Program::GetInstance().ExitGuiScreen();
}

void SaveFileScreen::SerializeScene(const std::string& path, const std::string& name) {

	Project& project = Program::GetProject();

	LOG("saving to \"%s\"...", path.c_str());
	std::ofstream ofs(path);
	boost::archive::text_oarchive oa(ofs);
	oa << Serialization(
			Linq::Select<std::shared_ptr<ModelObject>, ModelObject*>(project.GetModelObjects(), [](auto objSharedPtr){ return objSharedPtr.get(); }),
			MainScreen::GetComponents().sceneView3D.GenPreviewSnapshot());

	project.MakeExisting(name, path);

	LOG("save successful!");
}

void SaveFileScreen::ReSerializeProject(Project& project) {
	SerializeScene(project.GetPath(), project.GetName());
}
