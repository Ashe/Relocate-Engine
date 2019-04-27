// Font.h
// Resource used for rendering text

#ifndef FONT_H
#define FONT_H

#include "Game.h"
#include "Scripting.h"

// Resource for text
class Font {
  public:

    // Allow the Font type to be made in Lua
    static void registerFontType() {

      // Register Texture type
      Game::lua.new_usertype<Font>("Font",
        sol::constructors<Font(const std::string&)>()
      );
    }

    // Constructor
    Font(const std::string& fp)
      : filepath_(fp) {
      loadFromFilepath();
    }

    // Get a pointer to the texture
    sf::Font& getFont() {
      return font_;
    }

  private:

    // Filepath to font
    const std::string filepath_;

    // Font for this resource to store
    sf::Font font_;

    // Load texture from filepath
    void loadFromFilepath() {
      if (!font_.loadFromFile(filepath_)) { 
        Console::log("[Error] Could not load font from path: %s", filepath_.c_str());
      }
    }
};

#endif
