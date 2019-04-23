// Sprite.cpp
// Component used to render an entity on screen

#include "Sprite.h"

// Constructor
Sprite::Sprite(float frameInterval, bool paused, bool looped)
  : flipX(false)
  , flipY(false)
  , texture_(nullptr)
  , animation_(nullptr)
  , frameTime_(sf::seconds(frameInterval))
  , currentTime_(sf::Time::Zero)
  , currentFrame_(0)
  , isPaused_(paused)
  , isLooped_(looped) 
  , colour_(sf::Color::White) 
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

// Set this sprite to play an animation
void 
Sprite::setAnimation(const Animation* animation) {
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
void 
Sprite::playAnimation(const std::string& name) {
  auto it = animationMap_.find(name);
  if (it != animationMap_.end()) {
    const Animation* animation = it->second;
    if (animation != nullptr && animation_ != animation) {
      setAnimation(animation);
    }
  }
  play();
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
    x = frame.left;
    y = frame.top;
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
      currentFrame_ = 0;
      if (!isLooped_) {
        isPaused_ = true;
      }
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
  const auto& size = getTextureSize();
  ImGui::Text("Sprite size: %f, %f", size.x, size.y);
  colourPicker();
  ImGui::PushItemWidth(-1);
  ImGui::PopItemWidth();
  ImGui::NextColumn();
}

// ImGui colour picker widget
void 
Sprite::colourPicker() {

  static ImVec4 color = ImVec4(colour_);

  static bool alpha_preview = true;
  static bool alpha_half_preview = false;
  static bool drag_and_drop = true;
  static bool options_menu = true;
  static bool hdr = false;
  int misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);

  ImGui::Text("Sprite Colour:");
  ImGui::SameLine(); 

  // Generate a dummy default palette. The palette will persist and can be edited.
  static bool saved_palette_init = true;
  static ImVec4 saved_palette[32] = { };
  if (saved_palette_init) {
    for (int n = 0; n < IM_ARRAYSIZE(saved_palette); ++n) {
      ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f, saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
      saved_palette[n].w = 1.0f; // Alpha
    }
    saved_palette_init = false;
  }

  static ImVec4 backup_color;
  bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
  ImGui::SameLine();
  if (open_popup) {
    ImGui::OpenPopup("picker");
    backup_color = color;
  }
  if (ImGui::BeginPopup("picker")) {
    ImGui::Text("Colour picker");
    ImGui::Separator();
    ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
    ImGui::SameLine();

    ImGui::BeginGroup(); // Lock X position
    ImGui::Text("Current");
    ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60,40));
    ImGui::Text("Previous");
    if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60,40)))
      color = backup_color;
    ImGui::Separator();
    ImGui::Text("Palette");
    for (int n = 0; n < IM_ARRAYSIZE(saved_palette); ++n) {
      ImGui::PushID(n);
      if ((n % 8) != 0)
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);
      if (ImGui::ColorButton("##palette", saved_palette[n], ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip, ImVec2(20,20)))
        color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

      // Allow user to drop colors into each palette entry
      // (Note that ColorButton is already a drag source by default, unless using ImGuiColorEditFlags_NoDragDrop)
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
          memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
          memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
        ImGui::EndDragDropTarget();
      }

      ImGui::PopID();
    }
    ImGui::EndGroup();
    ImGui::EndPopup();
  }

  // Check for changes to colour
  if (sf::Color(color) != getColour()) {
    Sprite::setColour(color);
  }
}
