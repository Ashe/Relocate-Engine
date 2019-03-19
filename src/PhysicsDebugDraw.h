// PhysicsDebugDraw.h
// A class to combine Box2D's physics with SFML's rendering

#ifndef PHYSICSDEBUGDRAW_H
#define PHYSICSDEBUGDRAW_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include "Game.h"

// Debug Event
struct DebugRenderPhysicsEvent {
  sf::RenderWindow& window;
};

class PhysicsDebugDraw : public b2Draw {
  public:
		PhysicsDebugDraw()
			: b2Draw() {}

    // Window to draw from
    sf::RenderWindow* window;

    // Scale
    static float scale;

    // Conversion functions
		static sf::Color GLColorToSFML(const b2Color &color, sf::Uint8 alpha = 255);
		static sf::Vector2f B2VecToSFVec(const b2Vec2 &vector, bool scaleToPixels = true);

    // Draw SFML objects
    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
};

#endif
