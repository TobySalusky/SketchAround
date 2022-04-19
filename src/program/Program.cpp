//
// Created by Tobiathan on 4/7/22.
//

#include "Program.h"
#include "../util/Controls.h"
#include "../gui/Markdown.h"
#include "../screens/auxiliary/SaveFileScreen.h"

Program* Program::instance = nullptr;

Program::Program() :
	input(*window.GetInput()),
	guiScreens({
           {GuiScreenType::MAIN_SCREEN, std::make_shared<MainScreen>(window)},
           {GuiScreenType::OPEN_MENU, std::make_shared<OpenFileScreen>()},
           {GuiScreenType::SAVE_MENU, std::make_shared<SaveFileScreen>()},
           {GuiScreenType::EXPORT_MENU, std::make_shared<ExportScreen>()},
           {GuiScreenType::CONTROLS_MENU, std::make_shared<ControlScreen>()},
   })
{
	Program::instance = this;
	Controls::Initialize(&input);

	glfwSwapInterval(0); // NOTE: Removes limit from FPS!

	ImGuiHelper::Initialize(window);
	Markdown::LoadFonts();

	BlendModes::GetManager().Init(); // TODO: rewrite blend-modes
}

void Program::Run() {
	while (!window.ShouldClose()) {

		windowWidth  = window.GetWidth();
        windowHeight = window.GetHeight();


		auto time = (float) glfwGetTime();
		float deltaTime = time - lastTime;

		input.Update();
		glfwPollEvents();

		Update(deltaTime);
		Render();
		Gui();
		PostEvents(deltaTime);

		lastTime = time;
		window.SwapBuffers(); // Swap the screen buffers
	}
}

void Program::Update(float deltaTime) {
	GlobalHotKeys();

	window.SetTitle(selectedProject->GenWindowTitle().c_str());

	guiScreens[currentGuiScreen]->Update(deltaTime);
}

void Program::PostEvents(float deltaTime) {
	// screen-specific post events
	guiScreens[currentGuiScreen]->PostUpdate(deltaTime);

	// Global post events
	justEnteredGuiScreen = false;
}


void Program::HandleWindowResize() {
	// TODO: implement
}

void Program::OpenGuiScreen(GuiScreenType guiScreen) {
	currentGuiScreen = guiScreen;
	justEnteredGuiScreen = true;
}

bool Program::IsInMainScreen() const {
	return currentGuiScreen == GuiScreenType::MAIN_SCREEN;
}

void Program::ExitGuiScreen() {
	currentGuiScreen = GuiScreenType::MAIN_SCREEN;
}

void Program::GlobalHotKeys() {
	if (Controls::Check(CONTROLS_Save)) SaveHotKey();
	if (Controls::Check(CONTROLS_SaveAs)) OpenGuiScreen(GuiScreenType::SAVE_MENU);
	if (Controls::Check(CONTROLS_OpenFileOpenMenu)) OpenGuiScreen(GuiScreenType::OPEN_MENU);
	if (Controls::Check(CONTROLS_OpenExportMenu)) OpenGuiScreen(GuiScreenType::EXPORT_MENU);
	if (Controls::Check(CONTROLS_OpenControlsMenu)) OpenGuiScreen(GuiScreenType::CONTROLS_MENU);
}

void Program::Render() {
	guiScreens[currentGuiScreen]->Render();
}

void Program::Gui() {

	ImGuiHelper::BeginFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

	guiScreens[currentGuiScreen]->Gui();

	ImGuiHelper::EndFrame();
}

void Program::Terminate() { // NOTE: called on destruction
	// Manual Resource Clears
	ImGuiHelper::Destroy();
	glfwTerminate();
}

Program::~Program() {
	Terminate();
}

void Program::AddProjectAsActive(const std::shared_ptr<Project>& newProject) {
	projects.push_back(newProject);
	selectedProject = newProject;
}

void Program::SaveHotKey() {
	if (selectedProject->IsUntitled()) {
		OpenGuiScreen(GuiScreenType::SAVE_MENU);
	} else {
		SaveFileScreen::ReSerializeProject(GetProject());
	}
}

