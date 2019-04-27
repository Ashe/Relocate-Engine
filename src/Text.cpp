// Text.cpp
// Component which renders text

#include "Text.h"

// Initialise static variables
std::string Text::defaultFontName_;

// Gets a font from the resource manager for this component to use
bool 
Text::setFontFromResources(const std::string& fontName) {

  // Easy outs
  if (fontName == "") { return false; }

  // Attempts to get the resource
  Resource& resource = ResourceManager::getResource(fontName);
  if (resource.getType() != Resource::Type::FONT) { 
    Console::log("[Error] Could not apply font: %s\nNonexistant or incorrect resource type.", fontName.c_str());
    return false; 
  }

  // Get font from resource
  Font* font = (Font*)resource.get();
  if (font == nullptr) { 
    Console::log("[Error] Could not apply font: %s\nResource is NULL..", fontName.c_str());
    return false; 
  }

  // Set the font of this text
  setFont(font->getFont());
  return true;
}

// Shows debug information to ImGui
void
Text::showDebugInformation() {
  ImGui::NextColumn();
  ImGui::Text("Text: %s", getString().toAnsiString().c_str());
  ImGui::Text("Size: %u", getCharacterSize());

  ImGui::Text("Fill Colour:"); ImGui::SameLine(); 
  const auto fillColour = getFillColor();
  auto col = showColourPicker(fillColour, "1"); 
  if (fillColour != col) { setFillColor(col); }
  ImGui::NewLine();

  ImGui::Text("Outline Colour:"); ImGui::SameLine(); 
  const auto outlineColour = getOutlineColor();
  col = showColourPicker(outlineColour, "2"); 
  if (outlineColour != col) { setOutlineColor(col); }
  ImGui::NewLine();

  ImGui::PushItemWidth(-1);
  ImGui::PopItemWidth();
  ImGui::NextColumn();
}
