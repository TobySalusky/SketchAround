//
// Created by Tobiathan on 4/11/22.
//

#ifndef SENIORRESEARCH_PLOT_H
#define SENIORRESEARCH_PLOT_H


#include "../gl/shaders/Shader2D.h"
#include "SceneView3D.h"

class Plot {
public:
	explicit Plot(const GLWindow& window);

	void Update(Project &project, float deltaTime);
	void PostUpdate(const Project &project, float deltaTime);
	void Render(const Project& project);
	void Gui();
	void ToolbarGui(Project& project);

	void HotKeys();

	[[nodiscard]] Enums::DrawMode GetDrawMode() const { return drawMode; }
private:
	void HandleUndoing();
	void LayerClearing(Project& project);

	Enums::DrawMode drawMode = Enums::MODE_PLOT;
	Enums::EditingTool selectedTool = Enums::TOOL_BRUSH;

	Shader2D shader2D = Shader2D::Read("shaders/shader2D.vert", "shaders/shader2D.frag");
	RenderTarget graphScene; // initialized in constructor TODO: re-impl on resize
	GraphView graphView;
	Mesh2D plot;
	Rectangle plotRect; // set per-frame via gui
	EditingContext editContext;
	Vec2 onScreen {};
	bool graphFocused = false;
};


#endif //SENIORRESEARCH_PLOT_H
