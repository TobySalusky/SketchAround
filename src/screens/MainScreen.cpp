//
// Created by Tobiathan on 4/12/22.
//

#include "MainScreen.h"
#include "../program/Program.h"
#include "../display/ParamEditor.h"

MainScreen* MainScreen::instance = nullptr;

MainScreen::MainScreen(GLWindow& window) :
	window(window),
	plot(window),
	sceneView3D(window),
	timeline(window),
	components({
		plot, sceneView3D, timeline, sceneHierarchy
	})
{
	MainScreen::instance = this;
}

void MainScreen::Update(float deltaTime) {
	Project& project = Program::GetProject();

	// hotkeys
	HotKeys();

	// Updating
	timeline.SetActiveAnimator(project.GetCurrentModelObject()->GetAnimatorPtr());
	plot.Update(project, deltaTime);
	sceneView3D.Update(project, deltaTime);
}

// After GUI
void MainScreen::PostUpdate(float deltaTime) {

	Project& project = Program::GetProject();

	// POST UPDATE EVENTS
	timeline.Update({Program::GetInput(), deltaTime, plot.GetDrawMode(), *project.GetCurrentModelObject(), project.GetModelObjects(), sceneHierarchy.FocusModeActive()});
	project.GetCurrentModelObject()->UnDiffAll();
	plot.PostUpdate(project, deltaTime);
	sceneView3D.PostUpdate(project, deltaTime);
}

void MainScreen::Render() {

	Project& project = Program::GetProject();

	sceneView3D.Render(project);
	plot.Render(project);
	timeline.Render({plot.GetDrawMode(), Program::GetInput()});
}

void MainScreen::Gui() {

	Project& project = Program::GetProject();


	plot.Gui();
	sceneView3D.Gui(project);
	timeline.Gui();
	sceneHierarchy.Gui(project);
	plot.ToolbarGui(project);
	ParamEditor::Gui();
}

void MainScreen::HotKeys() {
	// ROOT ESCAPE HOTKEY
	if (DEVELOPER_MODE && Program::GetInput().Pressed(GLFW_KEY_ESCAPE)) window.Close(); // TODO: use CONTROLS

	plot.HotKeys();
}
