//
// Created by Tobiathan on 4/12/22.
//

#ifndef SENIORRESEARCH_MAINSCREEN_H
#define SENIORRESEARCH_MAINSCREEN_H

#include <vector>
#include "../project/Project.h"
#include "../display/Plot.h"
#include "../animation/Timeline.h"
#include "../display/SceneHierarchy.h"
#include "GuiScreen.h"

struct MainScreenComponents {
	Plot& plot;
	SceneView3D& sceneView3D;
	Timeline& timeline;
	SceneHierarchy& sceneHierarchy;
};

class MainScreen : public GuiScreen {
public:
	explicit MainScreen(GLWindow& window);

	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override;
	void Render() override;
	void Gui() override;

	[[nodiscard]] static MainScreenComponents& GetComponents() { return instance->components; }

private:
	void HotKeys();

	static MainScreen* instance;

	GLWindow& window;

	Plot plot;
	SceneView3D sceneView3D;
	Timeline timeline;
	SceneHierarchy sceneHierarchy = {};

	MainScreenComponents components;
};


#endif //SENIORRESEARCH_MAINSCREEN_H
