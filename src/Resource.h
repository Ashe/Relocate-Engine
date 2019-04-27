// Resource.h
// A resource handle for loading resources

#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

// Base class for all resource functionalities
class Resource {
  public:

    // Resource type
    enum Type {
      UNKNOWN,
      SCENE,
      TEXTURE,
      FONT,
      ANIMATION,
      SPELL
    };

    // Constructors
    Resource() : filepath_(""), type_(Type::UNKNOWN), resource_(nullptr) {}
    Resource(const std::string& fp);

    // Destructor
    ~Resource() {}

    // Get private variables
    Type getType() const { return type_; }
    std::string getName() const { return name_; }

    // Load and/or get the resource
    void* get();

    // Force release of resource
    void release();

    // Overload operators
    void* operator->();
    void* operator*();

  protected:

    // Filepath to resource descriptor
    std::string filepath_;

    // Name of the file
    std::string name_;

    // What type of resource is this
    Type type_;

    // The resource within
    void* resource_;

    // Delete the resource depending on it's type
    void deleteResource();
};

#endif
