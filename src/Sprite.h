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
      return true;
    }
};

#endif
