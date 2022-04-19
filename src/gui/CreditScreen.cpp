//
// Created by Tobiathan on 3/27/22.
//

#include "CreditScreen.h"
#include "../vendor/imgui_markdown.h"
#include "Markdown.h"
#include <string>


void CreditScreen::Gui() {
	const std::string markdownText =
			R"(# Acknowledgements
### Outside libraries used:
  * **Gui:**              [Dear ImGui](https://github.com/ocornut/imgui) & [imgui_markdown](https://github.com/juliettef/imgui_markdown)
  * **Serialization:**    [Boost](https://www.boost.org/)
  * **OpenGL Rendering:** [GLEW](http://glew.sourceforge.net/) & [GLFW](https://www.glfw.org/)
  * **Math:**             [GLM](https://github.com/g-truc/glm)
  * **Image Loading:**    [stb_image](https://github.com/nothings/stb)

### Font:
[JetBrains Mono](https://www.jetbrains.com/lp/mono/)
)";

	ImGui::Begin("Credits");
	Markdown::RenderMarkdown(markdownText);
	ImGui::End();
}
