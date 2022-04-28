//
// Created by Tobiathan on 4/27/22.
//

#include "TimelineScrollBar.h"
#include "../program/Program.h"

void TimelineScrollBar::Render(Mesh2D &canvas, Shader2D& shader2D) const {
	// consts
	constexpr static RGBA scrollBackgroundColour = {0.10f, 0.10f, 0.10f, 1.0f};
	constexpr static RGBA colour {0.24f, 0.24f, 0.24f, 1.0f};
	constexpr static RGBA edgeColour {0.35f, 0.35f, 0.35f, 1.0f};

	// setup
	RenderTarget::Bind(scene);
	shader2D.Enable();

	// render
	canvas.AddQuad({-1.0f, -1.0f}, {1.0f, 1.0f}, scrollBackgroundColour);

	canvas.AddQuad(Util::Remap01ToNP({start, 0.0f}), Util::Remap01ToNP({end, 1.0f}), colour);

	constexpr static float edgePixels = 10.0f;
	const float edgeWidth = edgePixels / guiRect.width;
	canvas.AddQuad(Util::Remap01ToNP({start, 0.0f}), Util::Remap01ToNP({start + edgeWidth, 1.0f}), edgeColour);
	canvas.AddQuad(Util::Remap01ToNP({end - edgeWidth, 0.0f}), Util::Remap01ToNP({end, 1.0f}), edgeColour);

	// finalize
	canvas.ImmediateClearingRender();
	shader2D.Disable();
	RenderTarget::Unbind();
}

void TimelineScrollBar::Update() {
	const Input& input = Program::GetInput();

	const Vec2 mouseNorm = Util::NormalizeToRect01(input.GetMouse(), guiRect);
	const float mouseX = mouseNorm.x;
	const bool mouseOverBar = mouseNorm.y >= 0.0f && mouseNorm.y <= 1.0f && mouseX >= start && mouseX <= end;

	// handle dragging
	bool hasTrackpadScroll = input.mouseScrollHorizontal != 0.0f;
	if (!drag && ((mouseOverBar && input.mousePressed) || hasTrackpadScroll)) {
		drag = {mouseX, {start, end}, hasTrackpadScroll};
	}

	if (drag) {
		const auto& [initX, initBounds, trackPad] = *drag;
		const float diff = [](const Input& input, bool trackPad, float mouseDiff){
			if (trackPad) return input.mouseScrollHorizontal * -0.01f; // trackpad scrolling
			return mouseDiff;
		}(input, trackPad, mouseX - initX);
		const float boundedDiff = std::clamp(diff, -initBounds.start, 1.0f - initBounds.end);

		start = initBounds.start + boundedDiff;
		end = initBounds.end + boundedDiff;

		if ((!trackPad && input.mouseUnpressed) || trackPad) {
			drag = std::nullopt;
		}
	}
}

TimelineView TimelineScrollBar::GenView() const {
	return {start * maxScrollArea, end * maxScrollArea};
}

void TimelineScrollBar::Gui() {
	const auto scrollBarDimens = Util::ToImVec(Util::ToVec(ImGui::GetContentRegionAvail()) - Vec2(8.0f, 0.0f));
	ImGui::ImageButton(scene.GetTexture(), scrollBarDimens, {0.0f, 1.0f}, {1.0f, 0.0f}, 0);
	guiRect = ImGuiHelper::ItemRect();
}
