// Texture.h
// Resource and Component for creation and rendering of sprites

#ifndef TEXTURE_H
#define TEXTURE_H

#include "Game.h"
#include "Scripting.h"

// A resource that a Sprite component will use
class Texture {
  public:

    // Allow the Texture type to be made in Lua
    static void registerTextureType() {

      // Register Texture type
      Game::lua.new_usertype<Texture>("Texture",
        sol::constructors<Texture(const std::string&)>()
      );
    }

    // Constructor
    Texture(const std::string& fp)
      : filepath_(fp) {
      loadFromFilepath();
    }

  private:

    // Filepath to texture
    const std::string filepath_;

    // Texture for this texture to store
    sf::Texture texture_;

    // Load texture from filepath
    void loadFromFilepath() {
      if (!texture_.loadFromFile(filepath_)) { 
        Console::log("[Error] Could not load texture from path: %s", filepath_.c_str());
      }
    }
};

#endif
