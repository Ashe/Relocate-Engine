// Sprite.h
// Component used to render an entity on screen

#ifndef SPRITE_H
#define SPRITE_H

#include "Game.h"
#include "Scripting.h"
#include "Texture.h"
#include "ResourceManager.h"

// Component used to render an entity
class Sprite : public sf::Sprite {
  public:

    // Make this component scriptable
    static void registerSpriteType() {
      
      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Sprite>("Sprite");

      // Add extra sprite functionality
      Game::lua.new_usertype<Sprite>("Sprite",
        "scale", sol::property(
          &sf::Sprite::getScale,
          [](Sprite& self, const sf::Vector2f& s) { self.setScale(s); }),
        "colour", sol::property(
          &sf::Sprite::getColor,
          &sf::Sprite::setColor),
        "origin", sol::property(
          &sf::Sprite::getOrigin,
          [](Sprite& self, const sf::Vector2f& o) { self.setOrigin(o); }),
        "scaledOrigin", sol::property(
          [](Sprite& self) { return self.scaleToWorld(self.getOrigin()); },
          [](Sprite& self, const sf::Vector2f& o) { self.setOrigin(self.scaleToLocal(o)); }),
        "textureSize", sol::property(&Sprite::getTextureSize),
        "setSpriteFromResources", &Sprite::setSpriteFromResources
      );
    }

    // Allow the sprite to be constructed from the resource manager
    bool setSpriteFromResources(const std::string& texName) {

      // Attempts to get the resource
      Resource& resource = ResourceManager::getResource(texName);
      if (resource.getType() != Resource::Type::TEXTURE) { return false; }

      // Get texture from resource
      Texture* tex = (Texture*)resource.get();
      if (tex == nullptr) { return false; }

      // Set this sprite's texture
      sf::Sprite::setTexture(tex->getTexture());

      // By default set origin to centre
      Sprite::setOrigin(scaleToLocal(sf::Vector2f(0.5f, 0.5f)));
      return true;
    }

    // Get the width and height of texture
    sf::Vector2f getTextureSize() const {
      const auto rect = getTextureRect();
      return sf::Vector2f(rect.width, rect.height);
    }

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      const auto& size = getTextureSize();
      const auto& col = Sprite::getColor();
      ImGui::Text("Sprite size: %f, %f", size.x, size.y);
      //ImGui::Text("Sprite colour: (%d, %d, %d, %d)", col.r, col.g, col.b, col.a);
      colourPicker();
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }

  private:

    // Make world coords (such as origin coords) relative to texture size
    sf::Vector2f scaleToLocal(const sf::Vector2f& c) const {
      const auto rect = Sprite::getTextureRect();
      return sf::Vector2f(c.x * rect.width, c.y * rect.height);
    }

    // Make local coords (such as scaled origin coords) relative to the world
    sf::Vector2f scaleToWorld(const sf::Vector2f& c) const {
      const auto rect = Sprite::getTextureRect();
      return sf::Vector2f(c.x / rect.width, c.y / rect.height);
    }

    // ImGui colour picker widget
    void colourPicker() {

      static ImVec4 color = ImVec4(Sprite::getColor());

      static bool alpha_preview = true;
      static bool alpha_half_preview = false;
      static bool drag_and_drop = true;
      static bool options_menu = true;
      static bool hdr = false;
      int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

      ImGui::Text("Sprite Colour:");
      ImGui::SameLine(); 

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
      bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
      ImGui::SameLine();
      if (open_popup) {
        ImGui::OpenPopup("picker");
        backup_color = color;
      }
      if (ImGui::BeginPopup("picker")) {
        ImGui::Text("Colour picker");
        ImGui::Separator();
        ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
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

      // Check for changes to colour
      if (sf::Color(color) != Sprite::getColor()) {
        Sprite::setColor(color);
      }
    }
};

#endif
