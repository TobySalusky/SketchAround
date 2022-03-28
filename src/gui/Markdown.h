//
// Created by Tobiathan on 3/26/22.
//

#ifndef SENIORRESEARCH_MARKDOWN_H
#define SENIORRESEARCH_MARKDOWN_H

#include <string>
#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui_markdown.h"

class Markdown {
private:

	static ImFont* H1;
	static ImFont* H2;
	static ImFont* H3;
	static ImFont* normalTextBold;
	static ImFont* normalTextItalic;

	static ImGui::MarkdownConfig mdConfig;
	static void LinkCallback(ImGui::MarkdownLinkCallbackData data);
	static inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data);

	static void ExampleMarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo, bool start);

public:
	static void LoadFonts(float fontSize = 18.0f);

	static void RenderMarkdown(const std::string& markdown);
};


#endif //SENIORRESEARCH_MARKDOWN_H
