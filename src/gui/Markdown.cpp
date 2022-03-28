//
// Created by Tobiathan on 3/26/22.
//

#include "Markdown.h"
#include "../util/Util.h"

ImFont* Markdown::H1 = nullptr;
ImFont* Markdown::H2 = nullptr;
ImFont* Markdown::H3 = nullptr;
ImFont* Markdown::normalTextBold = nullptr;
ImFont* Markdown::normalTextItalic = nullptr;

ImGui::MarkdownConfig Markdown::mdConfig = {};

void Markdown::ExampleMarkdownFormatCallback(const ImGui::MarkdownFormatInfo &markdownFormatInfo, bool start) {
	// Call the default first so any settings can be overwritten by our implementation.
	// Alternatively could be called or not called in a switch statement on a case by case basis.
	// See defaultMarkdownFormatCallback definition for further examples of how to use it.
	ImGui::defaultMarkdownFormatCallback(markdownFormatInfo, start);

	switch (markdownFormatInfo.type) {
		case ImGui::MarkdownFormatType::EMPHASIS: {
			switch (markdownFormatInfo.level) {
				case 1: {
					if (start) {
						const float grayness = 0.6f;
						ImGui::PushFont(normalTextItalic);
						ImGui::PushStyleColor(ImGuiCol_Text, {grayness, grayness, 1.0f, 1.0f});
					} else {
						ImGui::PopFont();
						ImGui::PopStyleColor(ImGuiCol_Text);
					}
					break;
				}
				case 2: {
					if (start) {
						ImGui::PushFont(normalTextBold);
					} else {
						ImGui::PopFont();
					}
					break;
				}
			}
			break;
		}
		default:
			break;
	}
}

void Markdown::LinkCallback(ImGui::MarkdownLinkCallbackData data) {
	std::string commandStr = std::string("open ") + std::string(data.link).substr(0, data.linkLength);
	system(commandStr.c_str());
	LOG("%s", commandStr.substr(0, commandStr.length() - 2).c_str());
}

ImGui::MarkdownImageData Markdown::ImageCallback(ImGui::MarkdownLinkCallbackData data) {
	LOG("Implement image callback please!");
	return ImGui::MarkdownImageData();
}

void Markdown::RenderMarkdown(const std::string &markdown) {
	// You can make your own Markdown function with your preferred string container and markdown config.
	mdConfig.linkCallback =         LinkCallback; // >> ????
	mdConfig.tooltipCallback =      nullptr;
	mdConfig.imageCallback =        nullptr; // >> ????
	mdConfig.linkIcon =             nullptr; // >> ????
	mdConfig.headingFormats[0] =    { H1, true };
	mdConfig.headingFormats[1] =    { H2, true };
	mdConfig.headingFormats[2] =    { H3, false };
	mdConfig.userData =             nullptr;
	mdConfig.formatCallback =       ExampleMarkdownFormatCallback;

	ImGui::Markdown(markdown.c_str(), markdown.length(), mdConfig);
}

void Markdown::LoadFonts(float fontSize) {
	ImGuiIO& io = ImGui::GetIO();
	// >> Base font should already be loaded

	// Bold headings H2 and H3
	H1 = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Bold.ttf", fontSize * 2.0f); // TODO: add BOLD font file
	H2 = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Bold.ttf", fontSize * 1.5f); // TODO: add BOLD font file
	H3 = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Bold.ttf", fontSize * 1.3f); // TODO: add BOLD font file
	normalTextBold = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-ExtraBold.ttf", fontSize); // TODO: add BOLD font file
	normalTextItalic = io.Fonts->AddFontFromFileTTF("fonts/JetBrainsMono-Italic.ttf", fontSize); // TODO: add Italic font file
}
