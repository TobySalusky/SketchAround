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
	constexpr static RGBA edgeColourHover {0.45f, 0.45f, 0.45f, 1.0f};
	constexpr static RGBA edgeColourDrag {0.55f, 0.55f, 0.55f, 1.0f};

	// setup
	RenderTarget::Bind(scene);
	shader2D.Enable();

	// render
	canvas.AddQuad({-1.0f, -1.0f}, {1.0f, 1.0f}, scrollBackgroundColour);

	canvas.AddQuad(Util::Remap01ToNP({start, 0.0f}), Util::Remap01ToNP({end, 1.0f}), colour);

	constexpr static float edgePixels = 10.0f;
	const float edgeWidth = edgePixels / guiRect.width; // TODO: make func

	const bool mouseOverBar = [&](){ // TODO: make func
		const Input& input = Program::GetInput();

		const Vec2 mouseNorm = Util::NormalizeToRect01(input.GetMouse(), guiRect);
		const float mouseX = mouseNorm.x;
		return mouseNorm.y >= 0.0f && mouseNorm.y <= 1.0f && mouseX >= start && mouseX <= end;
	}();

	const ScrollBarDragInfo::DragType hoverDragType = [&](){
		const Input& input = Program::GetInput();

		const Vec2 mouseNorm = Util::NormalizeToRect01(input.GetMouse(), guiRect);
		const float mouseX = mouseNorm.x;

		if (mouseX <= start + edgeWidth) return ScrollBarDragInfo::Start;
		if (mouseX >= end - edgeWidth) return ScrollBarDragInfo::End;
		return ScrollBarDragInfo::Bar;
	}();

	const auto GetEdgeColour = [&](ScrollBarDragInfo::DragType thisDragType) -> RGBA {
		if (drag && drag->dragType == thisDragType) return edgeColourDrag;
		if (mouseOverBar && hoverDragType == thisDragType) return edgeColourHover;
		return edgeColour;
	};

	canvas.AddQuad(Util::Remap01ToNP({start, 0.0f}), Util::Remap01ToNP({start + edgeWidth, 1.0f}), GetEdgeColour(ScrollBarDragInfo::Start));
	canvas.AddQuad(Util::Remap01ToNP({end - edgeWidth, 0.0f}), Util::Remap01ToNP({end, 1.0f}), GetEdgeColour(ScrollBarDragInfo::End));

	// finalize
	canvas.ImmediateClearingRender();
	shader2D.Disable();
	RenderTarget::Unbind();
}

void TimelineScrollBar::Update(float currentTime) {
	const Input& input = Program::GetInput();

	const Vec2 mouseNorm = Util::NormalizeToRect01(input.GetMouse(), guiRect);
	const float mouseX = mouseNorm.x;
	const bool mouseOverBar = mouseNorm.y >= 0.0f && mouseNorm.y <= 1.0f && mouseX >= start && mouseX <= end;

	constexpr static float edgePixels = 10.0f;
	const float edgeWidth = edgePixels / guiRect.width;
	
	// handle dragging
	bool hasTrackpadScroll = input.mouseScrollHorizontal != 0.0f && guiPanelHovered;
	if (!drag && ((mouseOverBar && input.mousePressed) || hasTrackpadScroll)) {
		const ScrollBarDragInfo::DragType dragType = [&](){

			if (!hasTrackpadScroll) {
				if (mouseX <= start + edgeWidth) return ScrollBarDragInfo::Start;
				if (mouseX >= end - edgeWidth) return ScrollBarDragInfo::End;
			}
			return ScrollBarDragInfo::Bar;
		}();
		drag = {mouseX, {start, end}, dragType, hasTrackpadScroll};
	}

	if (drag) {
		const auto& [initX, initBounds, dragType, trackPad] = *drag;
		const float diff = [](const Input& input, bool trackPad, float mouseDiff){
			if (trackPad) return input.mouseScrollHorizontal * -0.01f; // trackpad scrolling
			return mouseDiff;
		}(input, trackPad, mouseX - initX);

		// TODO: change this
		const auto& [diffStart, diffEnd] = [&, dragType=dragType, initBounds=initBounds]() -> std::tuple<float, float> {

			if (dragType != ScrollBarDragInfo::Bar) {
				// case wherein time is in between
				const float initTimeStart = TimeAtLocation(initBounds.start);
				const float initTimeEnd   = TimeAtLocation(initBounds.end);
				bool caretInRange = initTimeStart <= currentTime && currentTime <= initTimeEnd;
				
				float growth = std::abs(diff) * maxScrollArea;
				const float growDir = (float) Util::Signum(diff) * ((dragType == ScrollBarDragInfo::End) ? 1.0f : -1.0f);
				
				const float minTimeRange = 0.5f;
				if (growDir < 0.0f) growth = std::min(growth, initTimeEnd - initTimeStart - minTimeRange); // shrinking constraint
				
				const float initGapBefore = currentTime - initTimeStart;
				const float initGapAfter  = initTimeEnd - currentTime;
				
				const float inequality = std::abs(initGapAfter - initGapBefore);
				
				float growLeft = std::max(0.0f, growth - inequality) / 2.0f;
				float growRight = growLeft;
				
				const bool leftFirst = initGapBefore < initGapAfter;
				*(leftFirst ? &growLeft : &growRight) += std::min(growth, inequality);
				
				float diffStart = (growDir < 0.0f) ? growRight / maxScrollArea : -growLeft / maxScrollArea;
				float diffEnd = (growDir < 0.0f) ? -growLeft / maxScrollArea : growRight / maxScrollArea;
				
				bool fedBack = false;
				if (diffStart < -initBounds.start) {
					diffEnd += -initBounds.start - diffStart;
					diffStart = -initBounds.start;
					fedBack = true;
				}
				
				if (diffEnd > 1.0f - initBounds.end) {
					if (!fedBack) diffStart = std::max(-initBounds.start, diffStart - (diffEnd - (1.0f - initBounds.end)));
					diffEnd = 1.0f - initBounds.end;
				}
				
				return {diffStart, diffEnd};
			}

			const float diffBoth = std::clamp(diff, -initBounds.start, 1.0f - initBounds.end);
			return {diffBoth, diffBoth};
		}();

		start = initBounds.start + diffStart;
		end = initBounds.end + diffEnd;
		
		if (drag->dragType != ScrollBarDragInfo::Bar) {
			drag = {mouseX, {start, end}, drag->dragType, false};
		}

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

	if (drag && drag->dragType != ScrollBarDragInfo::Bar) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	}
	
	guiPanelHovered = ImGui::IsWindowHovered();
}

float TimelineScrollBar::TimeAtLocation(float f01) const {
	return f01 * maxScrollArea;
}
