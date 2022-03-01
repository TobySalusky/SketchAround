//
// Created by Tobiathan on 2/24/22.
//

#ifndef SENIORRESEARCH_GUISTYLE_H
#define SENIORRESEARCH_GUISTYLE_H


#include "../vendor/imgui/imgui.h"
#include <vector>

struct GuiColor {
    ImGuiCol_ colorType {};
    ImVec4 rgba {};
};


class GuiStyle {
public:
    GuiStyle() = default;

    explicit GuiStyle(const std::vector<GuiColor>& guiColors) {
        popCount = guiColors.size();
        for (const GuiColor& guiColor : guiColors) {
            ImGui::PushStyleColor(guiColor.colorType, guiColor.rgba);
        }
    }

    ~GuiStyle() {
        ImGui::PopStyleColor(popCount);
    }

    auto operator=( GuiStyle other )
    -> GuiStyle&
    {
        ImGui::PopStyleColor(popCount);
        std::swap( *this, other );
        return *this;
    }

private:
    int popCount = 0;
};

#endif //SENIORRESEARCH_GUISTYLE_H
