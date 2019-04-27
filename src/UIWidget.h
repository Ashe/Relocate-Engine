// UIWidget.h
// A class that all UI elements should have

#ifndef UIWIDGET_H
#define UIWIDGET_H

#include "Game.h"
#include "Scripting.h"

// Ensure that this widget is placed on the HUD and not in world space
class UIWidget : Component {
  public:

    // Make this component scriptable
    static void registerUIWidgetType(sol::environment& env) {

      // Register the usual assign, has, remove functions to Entity
      Script::registerComponentToEntity<UIWidget>(env, "UIWidget");

      // Create the UIWidget usertype
      env.new_usertype<UIWidget>("UIWidget",
        "anchor", &UIWidget::anchor
      );
    }

    // Constructors
    UIWidget(ECS::Entity* e)
      : Component(e)
      , anchor(sf::Vector2f(-1.f, -1.f)) {
    }

    // Where this widget is placed, relative to view
    sf::Vector2f anchor;

    // Shows the debug information to ImGui
    void showDebugInformation() {
      ImGui::NextColumn();
      ImGui::Text("HUD anchor: %f, %f", anchor.x, anchor.y);
      ImGui::PushItemWidth(-1);
      ImGui::PopItemWidth();
      ImGui::NextColumn();
    }
};

#endif
