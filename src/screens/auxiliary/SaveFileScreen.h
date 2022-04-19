//
// Created by Tobiathan on 4/14/22.
//

#ifndef SENIORRESEARCH_SAVEFILESCREEN_H
#define SENIORRESEARCH_SAVEFILESCREEN_H


#include "../GuiScreen.h"

#include "../GuiScreen.h"
#include "../../misc/Serialization.h"
#include "../../gl/TiledTextureAtlas.h"
#include <string>
#include <filesystem>

class SaveFileScreen : public GuiScreen {
public:
	void Update(float deltaTime) override;
	void Gui() override;

	static void ReSerializeProject(Project& project);

private:
	void TrySave();
	static void SerializeScene(const std::string& path, const std::string& name);

	TimedPopup popupWarning;
	char nameBuffer[256] = "";
	char pathBuffer[256] = "/Users/toby/ClionProjects/SeniorResearch/output";
};


#endif //SENIORRESEARCH_SAVEFILESCREEN_H
