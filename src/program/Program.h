//
// Created by Tobiathan on 4/7/22.
//

#ifndef SENIORRESEARCH_PROGRAM_H
#define SENIORRESEARCH_PROGRAM_H

#include <glad.h>
#include <glfw3.h>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include "../Enums.h"
#include "../util/Util.h"
#include "../editing/ProjectContext.h"
#include "../gl/GLWindow.h"
#include "../project/Project.h"
#include "../screens/MainScreen.h"
#include "../screens/GuiScreen.h"
#include "../screens/auxiliary/OpenFileScreen.h"
#include "../screens/auxiliary/SaveFileScreen.h"
#include "../screens/auxiliary/ExportScreen.h"
#include "../screens/auxiliary/ControlScreen.h"


// >> START CONSTANTS
const GLuint INIT_WIDTH = 2000;
const GLuint INIT_HEIGHT = 1000;

#define DEVELOPER_MODE true
// >> END CONSTANTS


// >> START ENUMS
enum class GuiScreenType {
	MAIN_SCREEN, SAVE_MENU, OPEN_MENU, EXPORT_MENU, CONTROLS_MENU
};
// >> END ENUMS


class Program {
public:
	Program(); // initialization
	~Program();
	void Run();

	[[nodiscard]] static Program& GetInstance() { return *Program::instance; };
	[[nodiscard]] static Input& GetInput() { return Program::instance->input; }
	[[nodiscard]] static Project& GetProject() { return *Program::instance->selectedProject; }
	[[nodiscard]] static unsigned int GetWindowWidth() { return Program::instance->windowWidth; }
	[[nodiscard]] static unsigned int GetWindowHeight() { return Program::instance->windowHeight; }
	[[nodiscard]] static bool JustEnteredGuiScreen() { return Program::instance->justEnteredGuiScreen; }
	void ExitGuiScreen();

	void AddProjectAsActive(const std::shared_ptr<Project>& newProject);

private:
	void Update(float deltaTime);
	void PostEvents(float deltaTime);
	void Render();
	void Gui();

	static void Terminate();

	void HandleWindowResize();
	void OpenGuiScreen(GuiScreenType guiScreen);
	void GlobalHotKeys();
	[[nodiscard]] bool IsInMainScreen() const;

	void SaveHotKey();

	static Program* instance;

	GLWindow window {INIT_WIDTH, INIT_HEIGHT}; // GLFW window
	Input& input;

	unsigned int windowWidth = INIT_WIDTH;
	unsigned int windowHeight = INIT_HEIGHT;

	float lastTime = 0.0f;

	GuiScreenType currentGuiScreen = GuiScreenType::MAIN_SCREEN;
	std::unordered_map<GuiScreenType, std::shared_ptr<GuiScreen>> guiScreens =
		{
			{GuiScreenType::OPEN_MENU, std::make_shared<OpenFileScreen>()},
			{GuiScreenType::SAVE_MENU, std::make_shared<SaveFileScreen>()},
			{GuiScreenType::EXPORT_MENU, std::make_shared<ExportScreen>()},
			{GuiScreenType::CONTROLS_MENU, std::make_shared<ControlScreen>()},
		};

	bool justEnteredGuiScreen = false;

	std::shared_ptr<Project> selectedProject = std::make_shared<Project>();
	std::vector<std::shared_ptr<Project>> projects = {std::shared_ptr<Project>(selectedProject)};
};


#endif //SENIORRESEARCH_PROGRAM_H
