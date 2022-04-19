//
// Created by Tobiathan on 4/14/22.
//

#include "ControlScreen.h"

#include "../../program/Program.h"
#include "../../util/Controls.h"

void ControlScreen::Gui() {

	ImGuiHelper::BeginOutsideGuiScreen("Export Menu");

	ImGuiHelper::GuiScreenExitBar();

	ImGuiHelper::SpacedSep();

	Controls::GUI();

	ImGui::End();
}