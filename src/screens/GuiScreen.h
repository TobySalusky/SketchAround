//
// Created by Tobiathan on 4/14/22.
//

#ifndef SENIORRESEARCH_GUISCREEN_H
#define SENIORRESEARCH_GUISCREEN_H


class GuiScreen {
public:
	virtual ~GuiScreen() = default;

	// optionally implementable
	virtual void Update(float deltaTime) {}

	// optionally implementable
	virtual void PostUpdate(float deltaTime) {}

	// optionally implementable
	virtual void Render() {}

	virtual void Gui() = 0; // mandatory implementation
};


#endif //SENIORRESEARCH_GUISCREEN_H
