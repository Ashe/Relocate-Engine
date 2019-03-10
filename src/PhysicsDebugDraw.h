// PhysicsDebugDraw.h
// A class to combine Box2D's physics with SFML's rendering

#ifndef PHYSICSDEBUGDRAW_H
#define PHYSICSDEBUGDRAW_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

#include "Game.h"

namespace sfdd {
	const float SCALE = 1.f;
}

class PhysicsDebugDraw : public b2Draw {
  public:
		PhysicsDebugDraw()
			: b2Draw() {}

		// Convert Box2D's OpenGL style color definition[0-1] to SFML's color definition[0-255]
		// with optional alpha byte[Default - opaque]
		static sf::Color GLColorToSFML(const b2Color &color, sf::Uint8 alpha = 255) {
			return sf::Color(static_cast<sf::Uint8>(color.r * 255), 
				static_cast<sf::Uint8>(color.g * 255), 
				static_cast<sf::Uint8>(color.b * 255), alpha);
		}

		// Convert Box2D's vector to SFML vector 
		// [Default - scales the vector up by SCALE constants amount]
		static sf::Vector2f B2VecToSFVec(const b2Vec2 &vector, bool scaleToPixels = true) {
			return sf::Vector2f(vector.x * (scaleToPixels ? sfdd::SCALE : 1.f), 
				vector.y * (scaleToPixels ? sfdd::SCALE : 1.f));
		}

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
    void DrawTransform(const b2Transform& xf) override;
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
};

#endif
