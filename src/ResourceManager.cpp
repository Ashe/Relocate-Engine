// ResourceManager.cpp
// A loader and container for resources

#include "ResourceManager.h"

// Avoid cyclic dependancies
#include "Game.h"
#include "Scripting.h"

// Initialise static members
std::map<std::string, Resource> ResourceManager::resources_;

// Registers resource types to files during initialisation
void
ResourceManager::registerResourceTypes() {
  Game::lua.set("Resource_UNKNOWN", Resource::Type::UNKNOWN);
  Game::lua.set("Resource_SCENE", Resource::Type::SCENE);
  Game::lua.set("Resource_SPELL", Resource::Type::SPELL);
}

// Import all files from a folder
void
ResourceManager::loadResources(const std::string& dir) {

  // For every file in the directory
  for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
    if (entry.is_regular_file()) {
      const std::string path = entry.path();
      Resource resource(path);
      const std::string name = resource.getName();
      if (resource.getType() != Resource::Type::UNKNOWN && name != "") {
        resources_[name] = resource;
        Console::log("Loaded resource: %s", name.c_str());
      }
    }
  }
}

// Get a resource by name, or a null resource
Resource
ResourceManager::getResource(const std::string& name) {
  auto it = resources_.find(name);
  if (it != resources_.end()) {
    Console::log("Found resource: %s", name.c_str());
    return it->second;
  }
  return Resource();
}

// Delete all resources
void
ResourceManager::releaseResources() {

  // Iterate through map
  for (auto i = resources_.begin(); i != resources_.end(); ++i) {
    Resource& r = i->second;
    r.release();
  }

  // Clear the map
  resources_.clear();
}
