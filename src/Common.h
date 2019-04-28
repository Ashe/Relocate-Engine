// Common.h
// Miscellaneous classes and functions

#ifndef COMMON_H
#define COMMON_H

#include "ECS.h"
#include "Sol.h"

// Events for IMGUI
struct addDebugMenuEntryEvent {};
struct addDebugInfoEvent {};

// Base class for all components
class Component {
  public:
    Component(ECS::Entity* e)
      : owner_(e) {}
    Component(const Component& other) : owner_(other.owner_) {}
    void operator=(const Component& other) {}
  protected:
    ECS::Entity* const owner_;
};

// ImGui colour picker widget
inline sf::Color 
showColourPicker(sf::Color colour, const std::string& popupName = "picker") {

  ImVec4 color = ImVec4(colour);

  static bool alpha_preview = true;
  static bool alpha_half_preview = false;
  static bool drag_and_drop = true;
  static bool options_menu = true;
  static bool hdr = false;
  int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

  // Generate a dummy default palette. The palette will persist and can be edited.
  static bool saved_palette_init = true;
  static ImVec4 saved_palette[32] = { };
  if (saved_palette_init) {
    for (int n = 0; n < IM_ARRAYSIZE(saved_palette); ++n) {
      ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
      saved_palette[n].w = 1.0f; // Alpha
    }
    saved_palette_init = false;
  }

  static ImVec4 backup_color;
  bool open_popup = ImGui::ColorButton(std::string("Colour##" + popupName).c_str(), color, misc_flags);
  ImGui::SameLine();
  if (open_popup) {
    ImGui::OpenPopup(popupName.c_str());
    backup_color = color;
  }
  if (ImGui::BeginPopup(popupName.c_str())) {
    ImGui::Text("Colour picker");
    ImGui::Separator();
    ImGui::ColorPicker4(("##picker" + popupName).c_str(), (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
    ImGui::SameLine();

    ImGui::BeginGroup(); // Lock X position
    ImGui::Text("Current");
    ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60,40));
    ImGui::Text("Previous");
    if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60,40)))
      color = backup_color;
    ImGui::Separator();
    ImGui::Text("Palette");
    for (int n = 0; n < IM_ARRAYSIZE(saved_palette); ++n) {
      ImGui::PushID(n);
      if ((n % 8) != 0)
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
      if (ImGui::ColorButton("##palette", saved_palette[n], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(20,20)))
        color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

      // Allow user to drop colors into each palette entry
      // (Note that ColorButton is already a drag source by default, unless using ImGuiColorEditFlags_NoDragDrop)
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
          memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
          memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
        ImGui::EndDragDropTarget();
      }

      ImGui::PopID();
    }
    ImGui::EndGroup();
    ImGui::EndPopup();
  }

  return sf::Color(color);
}

#endif
