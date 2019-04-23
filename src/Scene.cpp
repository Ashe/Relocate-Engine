// Scene.cpp
// Logic more oriented to the game goes here

#include "Scene.h"

// Avoid cyclic dependencies
#include "ControlSystem.h"
#include "Transform.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Possession.h"
#include "Camera.h"
#include "Stats.h"
#include "Movement.h"
#include "Abilities.h"

// Register scene functionality to Lua
void
Scene::registerSceneType() {
  Game::lua.new_usertype<Scene>("Scene",
    sol::constructors<Scene()>(),
    "onBegin", &Scene::onBegin_,
    "onShow", &Scene::onShow_,
    "onHide", &Scene::onHide_,
    "onUpdate", &Scene::onUpdate_,
    "onWindowEvent", &Scene::onWindowEvent_,
    "onQuit", &Scene::onQuit_
  );
}

// Constructor
Scene::Scene ()
  : hasBegun_(false)
  , world_(ECS::World::createWorld()) {
}

// Copy constructor
Scene::Scene(const Scene& other) 
  : hasBegun_(other.hasBegun_)
  , world_(ECS::World::createWorld())
  , onBegin_(other.onBegin_)
  , onShow_(other.onShow_)
  , onHide_(other.onHide_)
  , onUpdate_(other.onUpdate_)
  , onWindowEvent_(other.onWindowEvent_)
  , onQuit_(other.onQuit_) {
}

// Destructor
Scene::~Scene() {
  world_->destroyWorld();
}

// Called when the scene is started
void
Scene::begin() {

  // Try to call the begin function from this scene's lua
  if (onBegin_.valid()) {
    auto attempt = onBegin_();
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.begin():\n> %s", err.what());
    }
  }

  // Flag that the scene has started
  hasBegun_ = true;
}

// Run this scene's script to register it's functions
void
Scene::registerFunctions() {

  // Set up environment
  lua_ = sol::environment(Game::lua, sol::create, Game::lua.globals());
  Script::registerSceneFunctions(lua_, world_);

  // Expose the world in the scene
  Game::lua["World"] = lua_;

  // Add to autocomplete
  Console::addCommand("[Class] World");
  Console::addCommand("World.createEntity");
}

// When the screen is shown
void
Scene::showScene() {

  // Begin the scene if it hasn't yet
  if (!hasBegun_) {
    begin();
  }

  // Try to call the begin function from this scene's lua
  if (onShow_.valid()) {
    auto attempt = onShow_();
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.showScene():\n> %s", err.what());
    }
  }
}

// When the screen is hidden
void
Scene::hideScene() {
  if (onHide_.valid()) {
    auto attempt = onHide_();
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.hideScene():\n> %s", err.what());
    }
  }
}

// Update the game every frame
void
Scene::update(const sf::Time& dt) {

  // Call scene's update script
  if (onUpdate_.valid()) {
    auto attempt = onUpdate_(dt);
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.update():\n> %s", err.what());
    }
  }

  // Update the ECS
  world_->update(dt);
}

// Render the game every frame
void
Scene::render(sf::RenderWindow& window) {

  // Get every entity with a sprite
  world_->each<Sprite>( [&](ECS::Entity* e, ECS::ComponentHandle<Sprite> s) {
    const Sprite& sprite = s.get();
    window.draw(sprite);
  });

  // Do any debug-only rendering
  if (Game::getDebugMode()) {
    world_->emit<DebugRenderPhysicsEvent>({window});
  }
}

// Handle keypresses
void
Scene::handleEvent(const sf::Event& event) {

  // Send event to control system
  ControlSystem::handleInput(event);

  // Call scene's input script
  if (onWindowEvent_.valid()) {
    auto attempt = onWindowEvent_(event);
    if (!attempt.valid()) {
      sol::error err = attempt;
      Console::log("[Error] in Scene.handleEvent():\n> %s", err.what());
    }
  }
}

// When the game is quit
void
Scene::quit() {
  bool quitAnyway = !onQuit_.valid();
  if (!quitAnyway) {
    auto attempt = onQuit_();
    if (!attempt.valid()) {
      quitAnyway = true;
      sol::error err = attempt;
      Console::log("[Error] in Scene.quit():\n> %s", err.what());
    }
  }
  if (quitAnyway) {
    quitAnyway = false;
    Console::log("Terminating program.");
    Game::terminate();
  }
}

/////////////////////
// DEBUG FUNCTIONS //
/////////////////////

// Add entries to debug menu
static bool showEntityViewer_ = false;
void
Scene::addDebugMenuEntries() {

  // Add entity viewer
  ImGui::MenuItem("Entity Viewer", NULL, &showEntityViewer_);

  // Allow systems to add entries
  world_->emit<addDebugMenuEntryEvent>({});
}

// Add information to the main IMGUI debug window
void
Scene::addDebugInfoToDefault() {

  // Add to default window
  ImGui::Begin("Debug");
  ImGui::Text("Entities in system: %lu", world_->getCount());
  ImGui::End();

  // Show the entity viewer
  if (showEntityViewer_) {
    ImGui::Begin("Entity Viewer", &showEntityViewer_);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2,2));
    ImGui::Columns(2);
    ImGui::Separator();

    // For every entity show data
    bool isFirst = true;
    for (ECS::Entity* e : world_->all()) {
      auto id = e->getEntityId();

      ImGui::PushID(id);
      ImGui::AlignTextToFramePadding();

      bool node_open = ImGui::TreeNode("Entity", "%s_%lu", "Entity", id);
      if (node_open) {

        ImGui::NextColumn();
        if (!isFirst) { ImGui::Spacing(); }
        isFirst = false;
        ImGui::Text("Entity %lu:", id);
        ImGui::NextColumn();

        // Represent Every component
        // @TODO: Clean this up
        auto t = e->get<Transform>();
        if (t.isValid()) {
          ImGui::PushID(0);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None , "Transform")) {
            t->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto s = e->get<Sprite>();
        if (s.isValid()) {
          ImGui::PushID(1);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None , "Sprite")) {
            s->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto r = e->get<RigidBody>();
        if (r.isValid()) {
          ImGui::PushID(2);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "RigidBody")) {
            r->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto c = e->get<Camera>();
        if (c.isValid()) {
          ImGui::PushID(3);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "Camera")) {
            c->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto p = e->get<Possession>();
        if (p.isValid()) {
          ImGui::PushID(4);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "Possession")) {
            p->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto stats = e->get<Stats>();
        if (p.isValid()) {
          ImGui::PushID(5);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "Stats")) {
            stats->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto m = e->get<Movement>();
        if (m.isValid()) {
          ImGui::PushID(6);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "Movement")) {
            m->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }
        auto a = e->get<Abilities>();
        if (a.isValid()) {
          ImGui::PushID(7);
          if (ImGui::TreeNodeEx("Field", ImGuiTreeNodeFlags_None, "Abilities")) {
            a->showDebugInformation();
            ImGui::TreePop();
          }
          ImGui::PopID();
        }

        // After all components end the tree
        ImGui::TreePop();
      }

      // End the current entity
      ImGui::PopID();
    }

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::PopStyleVar();
    ImGui::End();
  }

  // Allow systems to add more info
  world_->emit<addDebugInfoEvent>({});
}
