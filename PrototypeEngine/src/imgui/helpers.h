#pragma once

#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "IconsFontAwesome5Pro.h"

#include <string>

// --------------------------------------------------------------------
//
//                          ^
//                          |
//                          |
//                          |
//                          |
//                          + PADDINGY
//                          |
//                          |
//                          |
//                          v
//
//         PADDINGX         +---------------+---------------+
//   <--------+-------->    |               |               |
//                          +---------------+---------------+
//
//                          +---------------+---------------+
//                          |               |               |
//                          +---------------+---------------+
// --------------------------------------------------------------------

#define PrototypeImguiVec1Widget(TITLE, TITLESIZE, COLOR, OPX, SIZE, PADDINGX, PADDINGY, X)                                      \
    {                                                                                                                            \
        f32 posx = ImGui::GetCursorPosX();                                                                                       \
        f32 posy = ImGui::GetCursorPosY();                                                                                       \
        ImGui::SetCursorPosX(posx + PADDINGX);                                                                                   \
        ImGui::SetCursorPosY(posy + PADDINGY);                                                                                   \
        posx = ImGui::GetCursorPosX();                                                                                           \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(TITLE X, ImVec4(COLOR, 1.0f), ImGuiColorEditFlags_NoTooltip, ImVec2(TITLESIZE, 21.0f));               \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::LabelText("##" TITLE " X label", X);                                                                              \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPX;                                                                                                                     \
    }

#define PrototypeImguiVec2Widget(TITLE, TITLESIZE, OPX, OPY, SIZE, PADDINGX, PADDINGY, X, Y)                                     \
    {                                                                                                                            \
        f32 posx = ImGui::GetCursorPosX();                                                                                       \
        ImGui::Text(TITLE);                                                                                                      \
        f32 posy = ImGui::GetCursorPosY();                                                                                       \
        ImGui::SetCursorPosX(posx + PADDINGX);                                                                                   \
        ImGui::SetCursorPosY(posy + PADDINGY);                                                                                   \
        posx = ImGui::GetCursorPosX();                                                                                           \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(TITLE X, ImVec4(PROTOTYPE_RED, 1.0f), ImGuiColorEditFlags_NoTooltip);                                 \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::LabelText("##" TITLE " X label", X);                                                                              \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPX;                                                                                                                     \
        ImGui::SameLine();                                                                                                       \
        posx += SIZE;                                                                                                            \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(TITLE Y, ImVec4(PROTOTYPE_GREEN, 1.0f), ImGuiColorEditFlags_NoTooltip);                               \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::LabelText("##" TITLE " Y label", Y);                                                                              \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPY;                                                                                                                     \
    }

#define PrototypeImguiVec2WidgetUntitled(TITLESIZE, OPX, OPY, PADDINGX, PADDINGY, X, Y)                                          \
    {                                                                                                                            \
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);                                                                \
        f32 pieceSize = (ImGui::GetContentRegionAvailWidth() - 1.0f * PADDINGX) / 3.0;                                           \
        f32 posx      = ImGui::GetCursorPosX();                                                                                  \
        ImGui::SetNextItemWidth(pieceSize);                                                                                      \
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_RED, 1.0f));                                                     \
        OPX;                                                                                                                     \
        ImGui::PopStyleColor();                                                                                                  \
        ImGui::SameLine();                                                                                                       \
        posx += pieceSize + PADDINGX;                                                                                            \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(pieceSize);                                                                                      \
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GREEN, 1.0f));                                                   \
        OPY;                                                                                                                     \
        ImGui::PopStyleColor();                                                                                                  \
        ImGui::PopStyleVar();                                                                                                    \
    }

#define PrototypeImguiVec3Widget(TITLE, TITLESIZE, OPX, OPY, OPZ, SIZE, PADDINGX, PADDINGY, X, Y, Z)                             \
    {                                                                                                                            \
        f32 posx = ImGui::GetCursorPosX();                                                                                       \
        ImGui::Text(TITLE);                                                                                                      \
        f32 posy = ImGui::GetCursorPosY();                                                                                       \
        ImGui::SetCursorPosX(posx + PADDINGX);                                                                                   \
        ImGui::SetCursorPosY(posy + PADDINGY);                                                                                   \
        posx = ImGui::GetCursorPosX();                                                                                           \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(X, ImVec4(PROTOTYPE_RED, 1.0f), ImGuiColorEditFlags_NoTooltip);                                       \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::Text(X);                                                                                                          \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPX;                                                                                                                     \
        ImGui::SameLine();                                                                                                       \
        posx += SIZE + 5.0f;                                                                                                     \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(Y, ImVec4(PROTOTYPE_GREEN, 1.0f), ImGuiColorEditFlags_NoTooltip);                                     \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::Text(Y);                                                                                                          \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPY;                                                                                                                     \
        ImGui::SameLine();                                                                                                       \
        posx += SIZE + 5.0f;                                                                                                     \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::ColorButton(Z, ImVec4(PROTOTYPE_BLUE, 1.0f), ImGuiColorEditFlags_NoTooltip);                                      \
        ImGui::SameLine();                                                                                                       \
        ImGui::SetCursorPosX(posx + 7.0f);                                                                                       \
        ImGui::Text(Z);                                                                                                          \
        ImGui::SameLine();                                                                                                       \
        posx += TITLESIZE;                                                                                                       \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(SIZE);                                                                                           \
        OPZ;                                                                                                                     \
    }

#define PrototypeImguiVec3WidgetUntitled(TITLESIZE, OPX, OPY, OPZ, PADDINGX, PADDINGY, X, Y, Z)                                  \
    {                                                                                                                            \
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);                                                                \
        f32 pieceSize = (ImGui::GetContentRegionAvailWidth() - 2.0f * PADDINGX) / 3.0;                                           \
        f32 posx      = ImGui::GetCursorPosX();                                                                                  \
        ImGui::SetNextItemWidth(pieceSize);                                                                                      \
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_RED, 1.0f));                                                     \
        OPX;                                                                                                                     \
        ImGui::PopStyleColor();                                                                                                  \
        ImGui::SameLine();                                                                                                       \
        posx += pieceSize + PADDINGX;                                                                                            \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(pieceSize);                                                                                      \
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_GREEN, 1.0f));                                                   \
        OPY;                                                                                                                     \
        ImGui::PopStyleColor();                                                                                                  \
        ImGui::SameLine();                                                                                                       \
        posx += pieceSize + PADDINGX;                                                                                            \
        ImGui::SetCursorPosX(posx);                                                                                              \
        ImGui::SetNextItemWidth(pieceSize);                                                                                      \
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_BLUE, 1.0f));                                                    \
        OPZ;                                                                                                                     \
        ImGui::PopStyleColor();                                                                                                  \
        ImGui::PopStyleVar();                                                                                                    \
    }

static ImVector<ImRect> s_GroupPanelLabelStack;

namespace ImGuiHelpers {

// #define PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS

static void
BeginGroupPanel(const char* name, const ImVec2& size, bool& is_open, ImFont* awesomeFont)
{
#ifdef PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS
    ImGui::BeginGroup();

    auto cursorPos   = ImGui::GetCursorScreenPos();
    auto itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();
    ImGui::BeginGroup();

    ImVec2 effectiveSize = size;
    if (size.x < 0.0f)
        effectiveSize.x = ImGui::GetContentRegionAvailWidth();
    else
        effectiveSize.x = size.x;
    ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    /*if (ImGui::CollapsingHeader(name)) {
        is_open = true;
    } else {
        is_open = false;
    }*/
    // if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) { is_open = !is_open; }
#endif // PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS

    ImGui::PushFont(awesomeFont);
    ImGui::TextColored(ImVec4(PROTOTYPE_BLUE, 1.0f), is_open ? ICON_FA_ANGLE_DOWN : ICON_FA_ANGLE_RIGHT);
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) { is_open = !is_open; }
    ImGui::PopFont();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::TextColored(ImVec4(PROTOTYPE_BLUE, 1.0f), " %s", name);
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) { is_open = !is_open; }

#ifdef PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS
    auto labelMin = ImGui::GetItemRectMin();
    auto labelMax = ImGui::GetItemRectMax();
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
    ImGui::BeginGroup();

    // ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x -= frameHeight;

    auto itemWidth = ImGui::CalcItemWidth();
    ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

    s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
#endif // PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS
}

static void
EndGroupPanel()
{
#ifdef PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS
    ImGui::PopItemWidth();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(PROTOTYPE_LIGHTGRAY, 1.00f));

    auto frameHeight = ImGui::GetFrameHeight();

    ImGui::EndGroup();

    // ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

    ImGui::EndGroup();

    auto itemMin = ImGui::GetItemRectMin();
    auto itemMax = ImGui::GetItemRectMax();
    // ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(90, 90, 90, 150), 0.0f);

    auto labelRect = s_GroupPanelLabelStack.back();
    s_GroupPanelLabelStack.pop_back();

    ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
    ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
    for (int i = 0; i < 4; ++i) {
        switch (i) {
            // left half-plane
            case 0:
                ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true);
                ImGui::GetWindowDrawList()->AddRect(
                  frameRect.Min, frameRect.Max, ImColor(ImVec4(PROTOTYPE_RED, 1.0f)), halfFrame.x);
                break;
            // right half-plane
            case 1:
                ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
                ImGui::GetWindowDrawList()->AddRect(
                  frameRect.Min, frameRect.Max, ImColor(ImVec4(PROTOTYPE_GREEN, 1.0f)), halfFrame.x);
                break;
            // top
            case 2:
                ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true);
                ImGui::GetWindowDrawList()->AddRect(
                  frameRect.Min, frameRect.Max, ImColor(ImVec4(PROTOTYPE_BLUE, 1.0f)), halfFrame.x);
                break;
            // bottom
            case 3:
                ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true);
                ImGui::GetWindowDrawList()->AddRect(
                  frameRect.Min, frameRect.Max, ImColor(ImVec4(PROTOTYPE_YELLOW, 1.0f)), halfFrame.x);
                break;
        }

        ImGui::PopClipRect();
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
    ImGui::GetCurrentWindow()->Size.x += frameHeight;

    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    ImGui::EndGroup();
#endif // PROTOTYPE_IMGUI_USE_BORDERED_TRAIT_VIEWS
}

} // namespace ImGuiHelpers