// Animation.cpp
// Resource for animating sprites

#include "Animation.h"

// Constructor
Animation::Animation() {}

// Add a frame to the animation
void 
Animation::addFrame(const sf::IntRect& rect) {
  frames_.push_back(rect);
}

// Get the size of the animation
std::size_t 
Animation::getSize() const {
  return frames_.size();
}

// Get the nth frame
const sf::IntRect& 
Animation::getFrame(std::size_t n) const {
  return frames_[n];
}
