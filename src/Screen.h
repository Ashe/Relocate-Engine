// Screen.h
// The base class for showing what's on screen

#ifndef SCREEN_H
#define SCREEN_H

#include <SFML/Graphics.hpp>

class Screen {
  public:
    virtual void showScreen() = 0;
    virtual void hideScreen() = 0;
    virtual void update(const sf::Time& dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void quit() = 0;
};

#endif
