// Expire.h
// Component to delete an entity when the timer runs out

#ifndef EXPIRE_H
#define EXPIRE_H

#include "Game.h"
#include "Scripting.h"

// Component designed to destroy it's owner
class Expire : Component {
  public:

    // Make this component scriptable
    static void registerExpireType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<Expire>(env, "Expire");

      // Create the Expire usertype
      env.new_usertype<Expire>("Expire",
        "time", sol::property(
          &Expire::getTimeLeft,
          &Expire::setTimeLeft)
      );

      // Add the easy expiry function
      env.set("expireEntity", &Expire::expireEntity);
    }

    // Convenience function for expiring an entity
    static void expireEntity(ECS::Entity* e, float time) {
      if (!e->has<Expire>()) {
        e->assign<Expire>(e, time);
      }
      else {
        auto exp = e->get<Expire>();
        if (exp->getTimeLeft() > time) {
          exp->setTimeLeft(time);
        }
      }
    }

    // Constructor
    Expire(ECS::Entity* e, float time = 0.f)
      : Component(e) 
      , secondsUntilExpire_(time) {
    }

    // Update the component, dissipate the time left
    bool update(const sf::Time& dt) {
      secondsUntilExpire_ -= dt.asSeconds();
      return secondsUntilExpire_ <= 0;
    }

    // Get the time left
    float getTimeLeft() const { 
      return secondsUntilExpire_; 
    }

    // Set the time left
    void setTimeLeft(float time) { 
      secondsUntilExpire_ = time; 
    }

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("Time until expiration: %f", secondsUntilExpire_);
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }

  private:

    // How long until this entity is deleted
    float secondsUntilExpire_;

};

#endif
