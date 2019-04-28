// Sprite.cpp
// Component used to render an entity on screen

#include "Sprite.h"

// Constructor
Sprite::Sprite(ECS::Entity* e, float frameInterval, bool paused, bool looped)
  : Component(e)
  , lockAnimation(false)
  , flipX(false)
  , flipY(false)
  , texture_(nullptr)
  , animation_(nullptr)
  , frameTime_(sf::seconds(frameInterval))
  , currentTime_(sf::Time::Zero)
  , currentFrame_(0)
  , isPaused_(paused)
  , isLooped_(looped) 
  , colour_(sf::Color::White) 
  , spriteSheetAnchor_(sf::Vector2i(0, 0))
  , size_(1.f, 1.f)
  , scale_(1.f, 1.f)
  , origin_(0.5f, 0.5f) {
}

// Allow the sprite to be constructed from the resource manager
bool 
Sprite::setSpriteFromResources(const std::string& texName) {

  // Easy outs
  if (texName == "") { return false; }

  // Attempts to get the resource
  Resource& resource = ResourceManager::getResource(texName);
  if (resource.getType() != Resource::Type::TEXTURE) { 
    Console::log("[Error] Could not apply sprite: %s\nNonexistant or incorrect resource type.", texName.c_str());
    return false; 
  }

  // Get texture from resource
  Texture* tex = (Texture*)resource.get();
  if (tex == nullptr) { 
    Console::log("[Error] Could not apply sprite: %s\nResource is NULL..", texName.c_str());
    return false; 
  }

  // Set this sprite's texture
  texture_ = &tex->getTexture();

  // Prepare the sprite for drawing
  updateSprite();
  return true;
}

// Allow the animation to be retrieved from the resource manager
bool
Sprite::addAnimationFromResources(const std::string& name, const std::string& animationName) {

  // Easy outs
  if (name == "" && animationName == "") { return false; }

  // Attempts to get the resource
  Resource& resource = ResourceManager::getResource(animationName);
  if (resource.getType() != Resource::Type::ANIMATION) { 
    Console::log("[Error] Could not add animation: %s\nNonexistant or incorrect resource type.", animationName.c_str());
    return false; 
  }

  // Get texture from resource
  const Animation* animation = (Animation*)resource.get();
  if (animation == nullptr) { 
    Console::log("[Error] Could not add animation: %s\nResource is NULL..", animationName.c_str());
    return false; 
  }

  // Add the animation to the animation map
  animationMap_[name] = animation;
  return true;
}

// Get the animation that is currently playing
const Animation* 
Sprite::getAnimation() const {
  return animation_;
}

// Set this sprite to play an animation, also reset callback
void 
Sprite::setAnimation(const Animation* animation) {
  if (lockAnimation) { return; }
  resetCallback();
  animation_ = animation;
  currentFrame_ = 0;
  updateSprite();
}

// Get delay between frames
sf::Time 
Sprite::getFrameTime() const {
  return frameTime_;
}

// Set delay between frames
void 
Sprite::setFrameTime(const sf::Time& time) {
  frameTime_ = time;
}

// Check if the animation is playing
bool
Sprite::isPlaying() const {
  return !isPaused_;
}

// Play the currently set animation
void
Sprite::play() {
  isPaused_ = false;
}

// Play the given animation
bool 
Sprite::playAnimation(const std::string& name, bool restart) {
  bool success = false;
  auto it = animationMap_.find(name);
  if (it != animationMap_.end()) {
    const Animation* animation = it->second;
    if (animation != nullptr) {
      if (animation != animation_) {
        if (isLooped_) { 
          play(); 
          if (restart) { 
            currentFrame_ = 0; 
          }
        }
        setAnimation(animation);
      }
      success = true;
    }
  }

  // Return whether the animation was found
  return success;
}

// Play an animation with a callback
bool 
Sprite::playAnimationWithCallback(const std::string& name, std::function<void()> callback) {

  // Attempt to play given animation
  bool success = playAnimation(name);

  // Set callback if we succeeded
  if (success) { 
    callback_ = callback;
  }

  // Otherwise, do the callback immediately
  else if (callback) {
    callback();
  }

  // Return whether successful
  return success;
}

// Pause the current animation
void
Sprite::pause() {
  isPaused_ = true;
}

// Check if the animation is looping
bool
Sprite::isLooping() const {
  return isLooped_;
}

// Set whether to loop the animation
void
Sprite::setLooped(bool looped) {
  isLooped_ = looped;
}

// Get the current colour
sf::Color
Sprite::getColour() const {
  return colour_;
}

// Set the colour of the sprite
void 
Sprite::setColour(const sf::Color& colour) {

  // Easy out
  if (colour == colour_) { return; }

  // Set the new colour
  colour_ = colour;

  // Update the vertices' color
  vertices_[0].color = colour_;
  vertices_[1].color = colour_;
  vertices_[2].color = colour_;
  vertices_[3].color = colour_;
}

// Get the local bounds of the sprite
sf::FloatRect 
Sprite::getLocalBounds() const {

  // If there's an animation use the bounds provided
  float x = 0.f, y = 0.f;
  if (animation_ != nullptr) {
    const sf::IntRect frame = animation_->getFrame(currentFrame_);
    x = spriteSheetAnchor_.x + frame.left;
    y = spriteSheetAnchor_.y + frame.top;
  }

  // Otherwise, use the size of the sprite
  return sf::FloatRect(x, y, size_.x, size_.y);
}

// Get the bounds of the sprite in world space
sf::FloatRect 
Sprite::getGlobalBounds() const {
  return getTransform().transformRect(getLocalBounds());
}

// Set which frame to play from
void 
Sprite::updateSprite() {

  // Set up vertices in local space with respect to the origin
  sf::Vector2f originOffset = sf::Vector2f(origin_.x * size_.x * scale_.x, origin_.y * size_.y * scale_.y);
  vertices_[0].position = sf::Vector2f(- originOffset.x, - originOffset.y);
  vertices_[1].position = sf::Vector2f(- originOffset.x, (1.f - originOffset.y) + size_.y * scale_.y);
  vertices_[2].position = sf::Vector2f((1.f - originOffset.x) + size_.x * scale_.x, (1.f - originOffset.y) + size_.y * scale_.y);
  vertices_[3].position = sf::Vector2f((1.f - originOffset.x) + size_.x * scale_.x, - originOffset.y);

  // Get the local bounds for the texture
  const auto rect = getLocalBounds();

  // Prepare texture coordinate values
  float left = rect.left + 0.0001f;
  float right = left + rect.width;
  float top = rect.top;
  float bottom = top + rect.height;

  // Flip the texture if needed
  if (flipX) { std::swap(left, right); }
  if (flipY) { std::swap(top, bottom); }

  // Apply texture coordinates
  vertices_[0].texCoords = sf::Vector2f(left, top);
  vertices_[1].texCoords = sf::Vector2f(left, bottom);
  vertices_[2].texCoords = sf::Vector2f(right, bottom);
  vertices_[3].texCoords = sf::Vector2f(right, top);
}

// Update the animation
void
Sprite::updateAnimation(const sf::Time& dt) {

  // if not paused and we have a valid animation
  if (isPaused_ || animation_ == nullptr || frameTime_ == sf::Time::Zero) { return; }

  // Add delta time
  currentTime_ += dt;

  // If current time is bigger then the frame time advance one frame
  if (currentTime_ >= frameTime_) {

    // Reset time, but keep the remainder
    currentTime_ = sf::microseconds(currentTime_.asMicroseconds() % frameTime_.asMicroseconds());

    // Get next Frame index
    if (currentFrame_ + 1 < animation_->getSize()) {
      ++currentFrame_;
    }
    else {

      // Call the callback
      if (callback_) { callback_(); }

      // Loop back round or freeze
      if (isLooped_) { 
        currentFrame_ = 0; 
      }
      else { isPaused_ = true; }
    }

    // Update the sprite with the new frame
    updateSprite();
  }
}

// Get the width and height of texture
sf::Vector2f 
Sprite::getTextureSize() const {
  const auto rect = getLocalBounds();
  return sf::Vector2f(rect.width, rect.height);
}

// Make world coords (such as origin coords) relative to texture size
sf::Vector2f 
Sprite::scaleToLocal(const sf::Vector2f& c) const {
  const auto rect = getLocalBounds();
  return sf::Vector2f(c.x * rect.width, c.y * rect.height);
}

// Make local coords (such as scaled origin coords) relative to the world
sf::Vector2f 
Sprite::scaleToWorld(const sf::Vector2f& c) const {
  const auto rect = getLocalBounds();
  return sf::Vector2f(c.x / rect.width, c.y / rect.height);
}

// Render this sprite
void 
Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  if (texture_ != nullptr) {
    states.transform *= getTransform();
    states.texture = texture_;
    target.draw(vertices_, 4, sf::Quads, states);
  }
}

///////////////////
// DEBUG SECTION //
///////////////////

// Shows the debug information to ImGui
void 
Sprite::showDebugInformation() {
  ImGui::NextColumn();
  ImGui::Text("Sprite size: %f, %f", size_.x, size_.y);
  ImGui::Text("Sprite scale: %f, %f", scale_.x, scale_.y);
  ImGui::Text("Sprite origin: %f, %f", origin_.x, origin_.y);
  ImGui::Text("Sprite Colour:"); ImGui::SameLine(); 
  auto originalColour = getColour();
  auto col = showColourPicker(originalColour); 
  ImGui::NewLine();
  ImGui::Text("Is playing: %s", !isPaused_ ? "true" : "false");
  ImGui::Text("Is looping: %s", isLooped_ ? "true" : "false");
  ImGui::Text("Frame: %d", currentFrame_);
  ImGui::Text("Frame interval: %f", frameTime_);
  ImGui::Text("Is locked: %s", lockAnimation ? "true" : "false");
  ImGui::PushItemWidth(-1);
  ImGui::PopItemWidth();
  ImGui::NextColumn();

  // Write the colour if we need to
  if (col != originalColour) {
    setColour(col);
  }
}

