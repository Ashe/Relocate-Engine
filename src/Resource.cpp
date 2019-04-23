// Resource.cpp
// A resource handle for loading resources

#include "Resource.h"

// Avoid cyclic dependancies
#include "Game.h"
#include "Scripting.h"

#include "Scene.h"
#include "Texture.h"
#include "Animation.h"
#include "Spell.h"

// Get resource type from descriptor
Resource::Resource(const std::string& fp)
  : filepath_(fp)
  , type_(Type::UNKNOWN)
  , resource_(nullptr) {

  // Try to execute the file
  std::tuple<Resource::Type, std::string, sol::object> result;
  auto attempt = Game::lua.script_file(filepath_, &sol::script_pass_on_error);
  if (!attempt.valid()) {
    sol::error err = attempt;
    Console::log("[Error] in %s:\n> %s", filepath_.c_str(), err.what());
    return;
  }

  // If it was valid, return the type
  result = attempt;
  type_ = std::get<0>(result);
  name_ = std::get<1>(result);
}

// Load and/or get the resource
void*
Resource::get() {

  // Easy out if resource is loaded
  if (resource_ != nullptr) return resource_;

  // Ensure resource_ is nullptr to start with
  resource_ = nullptr;

  // Otherwise, execute the file
  std::tuple<Resource::Type, std::string, sol::object> result;
  auto attempt = Game::lua.script_file(filepath_, &sol::script_pass_on_error);
  if (!attempt.valid()) {
    sol::error err = attempt;
    Console::log("[Error] in %s:\n> %s", filepath_.c_str(), err.what());
    return resource_;
  }

  // Interpret and cast the second
  result = attempt;
  type_ = std::get<0>(result);
  sol::object data = std::get<2>(result);
  switch (type_) {
    case Type::SCENE:
      resource_ = new Scene(data.as<Scene>()); break;
    case Type::TEXTURE:
      resource_ = new Texture(data.as<Texture>()); break;
    case Type::ANIMATION:
      resource_ = new Animation(data.as<Animation>()); break;
    case Type::SPELL:
      resource_ = new Spell(data.as<Spell>()); break;
    default:
      break;
  }

  // Return resource no matter what
  return resource_;
}

// Force release of resource
void 
Resource::release() {
  if (resource_ != nullptr) {
    deleteResource();
    resource_ = nullptr;
  }
}

// Get resource via *
void*
Resource::operator*() {
  return get();
}

// Get resource via ->
void*
Resource::operator->() {
  return get();
}

// Delete the resource depending on it's type
void 
Resource::deleteResource() {
  switch (type_) {
    case Type::SCENE:
      delete static_cast<Scene*>(resource_); break;
    case Type::TEXTURE:
      delete static_cast<Texture*>(resource_); break;
    case Type::ANIMATION:
      delete static_cast<Animation*>(resource_); break;
    case Type::SPELL:
      delete static_cast<Spell*>(resource_); break;
    default:
      break;
  }
}
