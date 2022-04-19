//
// Created by Tobiathan on 4/14/22.
//

#ifndef SENIORRESEARCH_OPENFILESCREEN_H
#define SENIORRESEARCH_OPENFILESCREEN_H


#include "../GuiScreen.h"
#include "../../misc/Serialization.h"
#include "../../gl/TiledTextureAtlas.h"
#include <string>
#include <filesystem>

class OpenFileScreen : public GuiScreen {
public:
	void Gui() override;

private:
	static void AddProjectFromDeserialized(const std::string& path);
	static Serialization DeserializeMetaInfo(const std::string& path);

	TiledTextureAtlas openFileTextureAtlas;

	char pathBuffer[256] = "/Users/toby/ClionProjects/SeniorResearch/output";
	std::string lastPath;
	std::vector<std::string> subFolders;
	std::vector<std::filesystem::path> paths;
};


#endif //SENIORRESEARCH_OPENFILESCREEN_H
