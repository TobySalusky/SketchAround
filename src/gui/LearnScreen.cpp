//
// Created by Tobiathan on 3/26/22.
//

#include "LearnScreen.h"
#include "../vendor/imgui_markdown.h"
#include "Markdown.h"
#include <string>

void LearnScreen::Gui() {
	const std::string markdownText =
R"(# Layers
The drawn line's *layer* determines what said line will do (and is reflected in the colour).
Only one can be selected at a time.

**NOTE:** the purpose of certain layers differ between generation methods.

Layers can be chosen via buttons on the *Layer-selector Panel* or with keys *[1-4]*

Here are the numbers, colours, and purposes of each layer:
  1. Colour: *Black*
      * Lathing:   **Primary Curve**
      * Side-view: **Bounding Curve**
  2. Colour: *Blue*
      * Lathing:   **Y-offset Function**
      * Side-view: **Manual Central Axis**
  3. Colour: *Green*
      * Lathing:   **Z-offset Function**
      * Side-view: **Automatic Central Axis** (Non-user-editable)
  4. Colour: *Pink*
      * Both:      **Secondary Cross Section**

# Editing

Use *[Ctrl + X]* to clear the current layer.
)";

	ImGui::Begin("Learn");
	Markdown::RenderMarkdown(markdownText);
	ImGui::End();
}
