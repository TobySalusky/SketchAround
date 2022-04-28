//
// Created by Tobiathan on 4/27/22.
//

#ifndef SENIORRESEARCH_TIMELINESCROLLBAR_H
#define SENIORRESEARCH_TIMELINESCROLLBAR_H


#include "../gl/Mesh2D.h"
#include "../util/Util.h"
#include "TimelineView.h"
#include "../gl/RenderTarget.h"
#include "../gl/shaders/Shader2D.h"


struct ScrollBarDragInfo {
	float initX;
	struct InitBounds {
		float start;
		float end;
	} initBounds;
	bool trackPad = false;
};

// TODO: use enum dragType to move edges in similar method

struct TimelineScrollBar {
public:
	RenderTarget scene;

	float start {}; ///< norm. [0-1]
	float end {}; ///< norm. [0-1]

	// internal
	float maxScrollArea = 4.0f;
	std::optional<ScrollBarDragInfo> drag;
	Rectangle guiRect = {};

	void Update();
	void Render(Mesh2D& canvas, Shader2D& shader2D) const;
	void Gui();

	[[nodiscard]] TimelineView GenView() const;
};


#endif //SENIORRESEARCH_TIMELINESCROLLBAR_H
