// Sprite.h
// Component used to render an entity on screen

#ifndef SPRITE_H
#define SPRITE_H

#include <map>

#include "Game.h"
#include "Scripting.h"

#include "ResourceManager.h"
#include "Texture.h"
#include "Animation.h"

// Component used to render an entity
class Sprite : Component, public Renderable, public sf::Transformable {
  public:

    // Make this component scriptable
    static void registerSpriteType(sol::environment& env) {
      
      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Sprite>(env, "Sprite");

      // Add extra sprite functionality
      env.new_usertype<Sprite>("Sprite",
        "depth", &Renderable::depth,
        "lock", &Sprite::lockAnimation,
        "flipX", &Sprite::flipX,
        "flipY", &Sprite::flipY,
        "size", &Sprite::size_,
        "origin", &Sprite::origin_,
        "scale", &Sprite::scale_,
        "colour", sol::property(
          &Sprite::getColour,
          &Sprite::setColour),
        "frameInterval", sol::property(
          &Sprite::getFrameTime,
          &Sprite::setFrameTime),
        "setSprite", &Sprite::setSpriteFromResources,
        "spritesheetAnchor", &Sprite::spriteSheetAnchor_,
        "updateSprite", &Sprite::updateSprite,
        "addAnimation", &Sprite::addAnimationFromResources,
        "loop", sol::property(
          &Sprite::isLooping,
          &Sprite::setLooped),
        "isPlaying", sol::property(&Sprite::isPlaying),
        "updateSprite", &Sprite::updateSprite,
        "setAnimation", &Sprite::setAnimation,
        "playAnimation", &Sprite::playAnimation,
        "play", &Sprite::play,
        "pause", &Sprite::pause
      );
    }

    // Constructor
    Sprite(ECS::Entity* e, float interval = 0.1f, bool paused = false, bool looped = true);

    // Don't change animation while this is true
    bool lockAnimation;

    // Whether we should flip the sprite horizontally
    bool flipX;

    // Whether we should flip the sprite vertically
    bool flipY;

    // Allow the sprite to be constructed from the resource manager
    bool setSpriteFromResources(const std::string& texName);

    // Allow the animation to be retrieved from the resource manager
    bool addAnimationFromResources(const std::string& actionName, const std::string& animationName);

    // Get the animation that is currently playing
    const Animation* getAnimation() const;

    // Set this sprite to play an animation, also reset callback
    void setAnimation(const Animation* animation);

    // Get delay between frames
    sf::Time getFrameTime() const;

    // Set delay between frames
    void setFrameTime(const sf::Time& time);

    // Check if the animation is playing
    bool isPlaying() const;

    // Play the currently set animation
    void play();

    // Play the given animation
    bool playAnimation(const std::string& name, bool restart = false);

    // Play an animation with a callback
    bool playAnimationWithCallback(const std::string& name, std::function<void()> callback);

    // Pause the current animation
    void pause();

    // Stop animating
    void stop();

    // Check if the animation is looping
    bool isLooping() const;

    // Set whether the current animation should loop
    void setLooped(bool looped);

    // Get whether we have finished the animation
    bool hasFinishedAnimation() const { return currentFrame_ >= (animation_ ? animation_->getSize() - 1 : 0); }

    // Get the current colour
    sf::Color getColour() const;

    // Set the colour of the sprite
    void setColour(const sf::Color& color);

    // Get the local bounds of the sprite
    sf::FloatRect getLocalBounds() const;

    // Get bounds of the sprite in world space
    sf::FloatRect getGlobalBounds() const;

    // Update how the sprite will be drawn
    void updateSprite();

    // Update animation
    void updateAnimation(const sf::Time& dt);

    // Reset the callback
    void resetCallback() { callback_ = std::function<void()>(); }

    // Get the width and height of texture
    sf::Vector2f getTextureSize() const;

    // Shows the debug information to ImGui
    void showDebugInformation();

  private:

    // Collection of animations
    std::map<std::string, const Animation*> animationMap_;

    // The texture used by this sprite
    const sf::Texture* texture_;

    // The animation to play
    const Animation* animation_;

    // Callback for when an animation finishes
    std::function<void()> callback_;

    // Delay between each frame (CANNOT BE ZERO)
    sf::Time frameTime_;

    // Time accumilator
    sf::Time currentTime_;

    // The current frame the animation is on
    std::size_t currentFrame_;

    // Whether the animation is paused
    bool isPaused_;

    // Whether the animation should loop on completion
    bool isLooped_;

    // Colour of the sprite
    sf::Color colour_;

    // Where the animation should be taken from on the sprite sheet
    sf::Vector2i spriteSheetAnchor_;

    // Get the size this object should be
    sf::Vector2f size_;

    // Scale of the sprite
    sf::Vector2f scale_;

    // Origin, where 0 is min and 1 is width / height
    sf::Vector2f origin_;

    // @TODO: Write this comment
    sf::Vertex vertices_[4];

    // Render this sprite
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    // Make world coords (such as origin coords) relative to texture size
    sf::Vector2f scaleToLocal(const sf::Vector2f& c) const;
    
    // Make local coords (such as scaled origin coords) relative to the world
    sf::Vector2f scaleToWorld(const sf::Vector2f& c) const;

    // ImGui colour picker widget
    void colourPicker();
};

#endif
