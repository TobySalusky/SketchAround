//
// Created by Tobiathan on 4/11/22.
//

#ifndef SENIORRESEARCH_SCENEVIEW3D_H
#define SENIORRESEARCH_SCENEVIEW3D_H


#include "../gl/shaders/Shader3D.h"
#include "../project/Project.h"
#include "../gl/Material.h"


class SceneView3D {
public:
	explicit SceneView3D(const GLWindow& window);

	void Render(Project& project);
	void Update(Project& project, float deltaTime);
	void PostUpdate(Project& project, float deltaTime);
	void Gui(const Project& project);

	std::vector<unsigned char> GenPreviewSnapshot();

private:
	constexpr static const float Z_NEAR = 0.1f, Z_FAR = 100.0f;

	Shader3D shader3D = Shader3D::Read("shaders/shader.vert", "shaders/shader.frag");

	Camera camera {};
	Light mainLight {{0.5f, 0.5f, 0.5f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 0.8f};
	Light line3DGizmoLight {{1.0f, 0.3f, 1.0f, 0.5f}, {-1.0f, -1.0f, -1.0f}, 1.0f};
	Material material {0.8f, 16};
	Mesh planeGizmo {};
	Rectangle displayRect; // updated on gui draw
	bool displayFocused = false; // updated on gui draw

	RenderTarget modelScene; // TODO: calc on resize
	glm::mat4 projection; // TODO: calc on resize
};


#endif //SENIORRESEARCH_SCENEVIEW3D_H
