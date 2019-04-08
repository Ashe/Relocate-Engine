// PhysicsDebugDraw.cpp
// A class to combine Box2D's physics with SFML's rendering

#include "PhysicsDebugDraw.h"

// Avoid cyclic dependencies
#include "PhysicsSystem.h"

// Convert Box2D's OpenGL style color definition[0-1] to SFML's color definition[0-255]
// with optional alpha byte[Default - opaque]
sf::Color 
PhysicsDebugDraw::GLColorToSFML(const b2Color &color, sf::Uint8 alpha) {
  return sf::Color(static_cast<sf::Uint8>(color.r * 255),
    static_cast<sf::Uint8>(color.g * 255),
    static_cast<sf::Uint8>(color.b * 255), alpha);
}

// Convert Box2D's vector to SFML vector 
// [Default - scales the vector up by SCALE constants amount]
sf::Vector2f 
PhysicsDebugDraw::B2VecToSFVec(const b2Vec2 &vector, bool scaleToPixels) {
  return sf::Vector2f(vector.x * (scaleToPixels ? PhysicsSystem::scale : 1.f),
    vector.y * (scaleToPixels ? PhysicsSystem::scale : 1.f));
}

void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	sf::ConvexShape polygon(vertexCount);
	sf::Vector2f center;
	for(int i = 0; i < vertexCount; i++) {
		//polygon.setPoint(i, SFMLDraw::B2VecToSFVec(vertices[i]));
		sf::Vector2f transformedVec = PhysicsDebugDraw::B2VecToSFVec(vertices[i]);
    // flooring the coords to fix distorted lines on flat surfaces
		polygon.setPoint(i, sf::Vector2f(std::floor(transformedVec.x), std::floor(transformedVec.y))); 
	}																								   // they still show up though.. but less frequently
	polygon.setOutlineThickness(-1.f);
	polygon.setFillColor(sf::Color::Transparent);
	polygon.setOutlineColor(PhysicsDebugDraw::GLColorToSFML(color));

	window->draw(polygon);
}

void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	sf::ConvexShape polygon(vertexCount);
	for(int i = 0; i < vertexCount; i++) {
		//polygon.setPoint(i, SFMLDraw::B2VecToSFVec(vertices[i]));
		sf::Vector2f transformedVec = PhysicsDebugDraw::B2VecToSFVec(vertices[i]);
    // flooring the coords to fix distorted lines on flat surfaces
		polygon.setPoint(i, sf::Vector2f(std::floor(transformedVec.x), std::floor(transformedVec.y))); 
	}																								   // they still show up though.. but less frequently
	polygon.setOutlineThickness(-1.0f);
	polygon.setFillColor(PhysicsDebugDraw::GLColorToSFML(color, 60));
	polygon.setOutlineColor(PhysicsDebugDraw::GLColorToSFML(color));

	window->draw(polygon);
}

void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	sf::CircleShape circle(radius * PhysicsSystem::scale);
	circle.setOrigin(radius * PhysicsSystem::scale, radius * PhysicsSystem::scale);
	circle.setPosition(PhysicsDebugDraw::B2VecToSFVec(center));
	circle.setFillColor(sf::Color::Transparent);
	circle.setOutlineThickness(-1.f);
	circle.setOutlineColor(PhysicsDebugDraw::GLColorToSFML(color));

	window->draw(circle);
}

void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	sf::CircleShape circle(radius * PhysicsSystem::scale);
	circle.setOrigin(radius * PhysicsSystem::scale, radius * PhysicsSystem::scale);
	circle.setPosition(PhysicsDebugDraw::B2VecToSFVec(center));
	circle.setFillColor(PhysicsDebugDraw::GLColorToSFML(color, 60));
	circle.setOutlineThickness(1.f);
	circle.setOutlineColor(PhysicsDebugDraw::GLColorToSFML(color));

	b2Vec2 endPoint = center + radius * axis;
	sf::Vertex line[2] = {
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(center), PhysicsDebugDraw::GLColorToSFML(color)),
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(endPoint), PhysicsDebugDraw::GLColorToSFML(color)),
	};

	window->draw(circle);
	window->draw(line, 2, sf::Lines);
}

void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	sf::Vertex line[] = {
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(p1), PhysicsDebugDraw::GLColorToSFML(color)),
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(p2), PhysicsDebugDraw::GLColorToSFML(color))
	};

	window->draw(line, 2, sf::Lines);
}

void PhysicsDebugDraw::DrawTransform(const b2Transform& xf) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

	float lineLength = 0.4f;

	/*b2Vec2 xAxis(b2Vec2(xf.p.x + (lineLength * xf.q.c), xf.p.y + (lineLength * xf.q.s)));*/
	b2Vec2 xAxis = xf.p + lineLength * xf.q.GetXAxis();
	sf::Vertex redLine[] = {
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(xf.p), sf::Color::Red),
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(xAxis), sf::Color::Red)
	};

	// You might notice that the ordinate(Y axis) points downward unlike the one in Box2D testbed
	// That's because the ordinate in SFML coordinate system points downward while the OpenGL(testbed) points upward
	/*b2Vec2 yAxis(b2Vec2(xf.p.x + (lineLength * -xf.q.s), xf.p.y + (lineLength * xf.q.c)));*/
	b2Vec2 yAxis = xf.p + lineLength * xf.q.GetYAxis();
	sf::Vertex greenLine[] = {
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(xf.p), sf::Color::Green),
		sf::Vertex(PhysicsDebugDraw::B2VecToSFVec(yAxis), sf::Color::Green)
	};

	window->draw(redLine, 2, sf::Lines);
	window->draw(greenLine, 2, sf::Lines);
}

void PhysicsDebugDraw::DrawPoint(const b2Vec2 & p, float32 size, const b2Color & color) {

  // Easy out
  if (!PhysicsSystem::showRigidBodies_) { return; }

  // Draw point (not implemented yet)
}
