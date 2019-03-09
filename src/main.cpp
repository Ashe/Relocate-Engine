#include "Config.h"

#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

int main(int argc, char* argv[]) {

  cout << "Version " << Build_VERSION_MAJOR << "." << Build_VERSION_MINOR << endl;

  sf::RenderWindow renderWindow(sf::VideoMode(640, 480), "Demo Game");

  sf::CircleShape circleShape(200);
  circleShape.setFillColor(sf::Color::Blue);

  while (renderWindow.isOpen()) {
    sf::Event Event;
    while (renderWindow.pollEvent(Event)) {
      if (Event.type == sf::Event::Closed)
        renderWindow.close();
    }

    renderWindow.clear();
    renderWindow.draw(circleShape);
    renderWindow.display();
  }
}
