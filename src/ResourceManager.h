// ResourceManager.h
// A loader and container for resources

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <filesystem>
#include <string>
#include <map>

#include "Resource.h"

// Manage resource handles
class ResourceManager {
  public:

    // Registers resource types to files during initialisation
    static void registerResourceTypes();

    // Import all files from a folder
    static void loadResources(const std::string& dir = "Assets/");

    // Get a resource by name
    static Resource& getResource(const std::string& name);

    // Delete all stored resources
    static void releaseResources();

  private:

    // Map of all resources
    static std::map<std::string, Resource> resources_;

    // 'NULL' Resource
    static Resource nullResource_;

};

#endif
