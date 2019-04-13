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
};

#endif
