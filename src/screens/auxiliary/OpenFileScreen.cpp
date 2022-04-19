//
// Created by Tobiathan on 4/14/22.
//

#include "OpenFileScreen.h"
#include "../../program/Program.h"

#include <iostream>
#include <fstream>

#include <vector>
#include <boost/archive/text_iarchive.hpp>

void OpenFileScreen::Gui() {

	ImGuiHelper::BeginOutsideGuiScreen("Open File");

	ImGuiHelper::GuiScreenExitBar();

	if (Program::JustEnteredGuiScreen()) {
		lastPath = "";
	}

	ImGuiHelper::SpacedSep();
	ImGui::InputTextWithHint("##Path-Open", "path:", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
	ImGuiHelper::SpacedSep();

	std::filesystem::path path = std::string(pathBuffer);
	bool pathExists = std::filesystem::exists(path) && std::filesystem::is_directory(path);

	if (path != lastPath) {
		subFolders.clear();
		paths.clear();
		if (pathExists) {
			for(auto& p : std::filesystem::directory_iterator(path)){
				if (std::filesystem::is_directory(p.path())) subFolders.emplace_back(p.path());
				if (p.path().extension() == ".mdl") paths.emplace_back(p.path());
			}
		}
		std::vector<Serialization> metaReads;
		metaReads.reserve(paths.size());
		for (const std::string& pathStr : paths) {
			auto& ref = metaReads.emplace_back(DeserializeMetaInfo(pathStr));
			const int desiredSize = 64 * 64 * 3;
			if (ref.img.size() != desiredSize) {
				LOG("[Warning]: Preview image is incorrect size!\n");
				ref.img.clear();
				ref.img.reserve(desiredSize);
				for (int i = 0; i < desiredSize; i++) {ref.img.emplace_back(0);}
			}
		}

		openFileTextureAtlas.Generate(64, Linq::Select<Serialization, std::vector<unsigned char>*>(metaReads, [](Serialization& refVal){
			return &(refVal.img);
		}));
	}

	bool reloadFolder = false;
	if (!pathExists) {
		ImGui::Text("Invalid path!");
	} else if (paths.empty() && subFolders.empty()) {
		ImGui::Text("Nothing here... ¯\\_(*_*)_/¯");
	} else {

		static float previewSize = 128.0f;
		static float padding = 16.0f;

		const float availWidth = ImGui::GetWindowContentRegionWidth();
		const int columnCount = std::max(1, (int) ((availWidth + padding) / (previewSize + padding)));

		ImGui::Columns(columnCount, nullptr, false);

		int childPathIndex = 0;
		for (const std::filesystem::path& childPath : paths) {
			const auto OnPress = [&] {
				AddProjectFromDeserialized(childPath);
				Program::GetInstance().ExitGuiScreen();
			};

			const auto& [uv1, uv2] = openFileTextureAtlas.GetCoords(childPathIndex);
			ImGui::ImageButton((void*)(intptr_t)openFileTextureAtlas.ID, {previewSize, previewSize}, Util::ToImVec(uv1), Util::ToImVec(uv2));
			if (ImGui::IsItemClicked()) OnPress();
			if (ImGui::IsItemHovered() && Program::GetInput().mouseRightPressed) {
				ImGui::OpenPopup(childPath.c_str());
			}
			if (ImGuiHelper::HoverDelayTooltip()) ImGui::SetTooltip("%s", childPath.c_str());
			if (ImGui::BeginPopup(childPath.c_str())) {

				if (ImGui::Button("Delete")) {
					std::filesystem::remove(childPath);
					reloadFolder = true;
				}

				ImGui::EndPopup();
			}
			childPathIndex++;
			{ // displays a shortened version of the filename
				// TODO: once filename vs. extension is serialized properly, this should be removed
				std::string fileName = childPath.filename();

				const auto index = (int) fileName.find_last_of('.');
				if (index != -1) {
					fileName = fileName.substr(0, index);
				}

				const std::string fullFileName = fileName;
				bool shortened = false;
				const float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;
				static float characterWidth = ImGui::CalcTextSize(" ").x; // (monospaced font)
				if (textWidth > previewSize) {
					fileName = fileName.substr(0, (int)(textWidth / characterWidth) - 1) + "-";
					shortened = true;
				}

				ImGui::Text("%s", fileName.c_str());
				if (shortened && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", fullFileName.c_str());
			}
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

	for (const auto& subFolder : subFolders) {
		ImGui::BulletText("%s", subFolder.c_str());
	}

	lastPath = path;
	if (reloadFolder) lastPath = "";

	ImGui::End();
}

Serialization OpenFileScreen::DeserializeMetaInfo(const std::string &path) {

	Serialization::SetReadMetaOnly(true);
	Serialization serialization;

	std::ifstream ifs(path);
	boost::archive::text_iarchive ia(ifs);
	ia >> serialization;
	Serialization::SetReadMetaOnly(false);

	return serialization;
}

void OpenFileScreen::AddProjectFromDeserialized(const std::string &path) {
	Program::GetInstance().AddProjectAsActive(std::make_shared<Project>(path));
};
