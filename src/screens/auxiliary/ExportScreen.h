//
// Created by Tobiathan on 4/14/22.
//

#ifndef SENIORRESEARCH_EXPORTSCREEN_H
#define SENIORRESEARCH_EXPORTSCREEN_H


#include "../GuiScreen.h"
#include "../../util/ImGuiHelper.h"

class ExportScreen : public GuiScreen {
public:
	void Update(float deltaTime) override;
	void Gui() override;

private:
	void TryObjExport();

	TimedPopup popupWarning;
	char exportNameBuffer[256] = "";
	char pathBuffer[256] = "/Users/toby/ClionProjects/SeniorResearch/output/obj";
};


#endif //SENIORRESEARCH_EXPORTSCREEN_H
