// Text.h
// Component which renders text

#ifndef TEXT_H
#define TEXT_H

#include "Game.h"
#include "Scripting.h"

#include "ResourceManager.h"
#include "Font.h"

// Renderable class for text
class Text : Component, public sf::Text {
  public:

    // Make this component scriptable
    static void registerTextType(sol::environment& env) {
      
      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Text>(env, "Text");

      // Add extra floating text functionality
      env.new_usertype<Text>("Text",
        "text", sol::property(
          [](const Text& self) { return std::string(self.getString()); },
          [](Text& self, const std::string& text) { self.setString(text); }),
        "size", sol::property(
          &sf::Text::getCharacterSize,
          &sf::Text::setCharacterSize),
        "lineSpacing", sol::property(
          &sf::Text::getLineSpacing,
          &sf::Text::setLineSpacing),
        "outlineThickness", sol::property(
          &sf::Text::getOutlineThickness,
          &sf::Text::setOutlineThickness),
        "fillColour", sol::property(
          &sf::Text::getFillColor,
          &sf::Text::setFillColor),
        "outlineColour", sol::property(
          &sf::Text::getOutlineColor,
          &sf::Text::setOutlineColor),
        "scale", sol::property(
          &sf::Text::getScale,
          [](Text& self, const sf::Vector2f& scale) { self.setScale(scale); }),
        "origin", sol::property(
          &sf::Text::getOrigin,
          [](Text& self, const sf::Vector2f& origin) { self.setOrigin(origin); }),
        "setRelativeOrigin", &Text::setRelativeOrigin,
        "centerText", &Text::centerText,
        "setFont", &Text::setFontFromResources
      );

      // Allow access to default font
      env.set_function("getDefaultFont", []() { return Text::defaultFontName_; } );
      env.set_function("setDefaultFont",
        [](const std::string& f) { 
          defaultFontName_ = f;
          Console::log("Default font set to %s", f.c_str()); }
      );
    }

    // Constructor
    Text(ECS::Entity* e)
      : Component(e) {
      setFontFromResources(defaultFontName_);
      setRelativeOrigin(0.5f, 0.5f);
    }

    // Gets a font from the resource manager for this component to use
    bool setFontFromResources(const std::string& font);

    // Sets the origin in relation to size of the text
    void setRelativeOrigin(float x, float y) {
      const sf::FloatRect size = getLocalBounds();
      setOrigin(x * size.width, y * size.height);
    }

    // Easily center the text
    void centerText() { 
      setRelativeOrigin(0.5f, 0.5f); 
    }

    // Shows the debug information to ImGui
    void showDebugInformation();

  private:

    // Font to use by default
    static std::string defaultFontName_;

};

#endif
