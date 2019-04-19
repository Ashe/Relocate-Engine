// Animation.h
// Resource for animating sprites

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Game.h"
#include "Scripting.h"

// A collection of frames used to animate
class Animation {
  public:

    // Allow the Texture type to be made in Lua
    static void registerAnimationType() {

      // Register Animation type
      Game::lua.new_usertype<Animation>("Animation",
        sol::constructors<Animation()>(),
        "size", sol::property(&Animation::getSize),
        "getFrame", &Animation::getFrame,
        "addFrame", &Animation::addFrame
      );
    }

    // Constructor
    Animation();

    // Add a frame to the animation
    void addFrame(const sf::IntRect& rect);

    // Get the size of the animation
    std::size_t getSize() const;

    // Get the nth frame
    const sf::IntRect& getFrame(std::size_t n) const;

  private:

    // Collection of frames
    std::vector<sf::IntRect> frames_;
};


#endif
