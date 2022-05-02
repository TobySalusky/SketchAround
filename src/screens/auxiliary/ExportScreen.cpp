//
// Created by Tobiathan on 4/14/22.
//

#include "ExportScreen.h"
#include "../../program/Program.h"
#include "../../exporting/ObjExporter.h"

void ExportScreen::Gui() {


	ImGuiHelper::BeginOutsideGuiScreen("Export Menu");

	ImGuiHelper::GuiScreenExitBar();

	if (Program::JustEnteredGuiScreen()) {
		memset(&exportNameBuffer[0], 0, sizeof(exportNameBuffer));
		ImGui::SetKeyboardFocusHere();
	}

	bool enterPressed = Program::GetInput().Pressed(GLFW_KEY_ENTER);

	ImGuiHelper::SpacedSep();
	ImGui::InputTextWithHint("##Name-Export", "name:", exportNameBuffer, IM_ARRAYSIZE(exportNameBuffer), ImGuiInputTextFlags_EnterReturnsTrue);

	bool exportTriggered = ImGui::IsItemFocused() && enterPressed; // enter during typing field
	ImGui::InputTextWithHint("##Path-Export", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
	exportTriggered = exportTriggered ||  ImGui::IsItemFocused() && enterPressed; // enter during typing field

	exportTriggered = ImGui::Button("Export As .OBJ") || exportTriggered; // button pressed

	if (exportTriggered) {
		TryObjExport();
	}

	popupWarning.RenderGUI();

	ImGui::End();
}

void ExportScreen::TryObjExport() {

	if (std::string(exportNameBuffer).empty()) {
		popupWarning.Open("Must set file name!");
		return;
	}
	if (std::string(pathBuffer).empty()) {
		popupWarning.Open("Must set file path!");
		return;
	}

	const auto ExportTo = [](const std::string& path, const std::string& content) {
		std::ofstream f(path);
		LOG("Exporting to \"%s\"...", path.c_str());
		if (!f.fail()) {
			f << content.c_str();
			LOG("export successful!");
		} else {
			LOG("export failed!");
		}
	};

	ExportTo(std::string(pathBuffer) + "/" + std::string(exportNameBuffer) + ".obj", ObjExporter::GenerateFileContents(Program::GetProject().GetModelObjects()));

	Program::GetInstance().ExitGuiScreen();
}

void ExportScreen::Update(float deltaTime) {
	popupWarning.Update(deltaTime);
}
