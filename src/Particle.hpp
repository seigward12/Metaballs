#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

namespace BoundsTransform {
typedef sf::FloatRect (*BoundsTransformFct)(const sf::FloatRect&);
}  // namespace BoundsTransform

class Particle : public sf::Drawable {
   public:
	explicit Particle(float radius);
	~Particle();

	void update(const sf::Time& dt, const sf::FloatRect& boundary);
	bool collideWithParticle(Particle& other,
							 const float restitutionCoefficient = 1.0f);

	void setPosition(sf::Vector2f position);
	void setVelocity(sf::Vector2f velocity);
	void setRadius(float radius);
	void setColor(sf::Color color);
	void setInfiniteMass(bool isInfiniteMass);

	sf::Vector2f getCenterPosition() const;
	sf::FloatRect getGlobalBounds() const;
	sf::Vector2f getVelocity() const;
	float getRadius() const;
	sf::Color getColor() const;
	sf::CircleShape getShape() const;

	bool isColliding(const Particle& other) const;

	static void setGlobalBoundsTransform(
		BoundsTransform::BoundsTransformFct boundsModification);
	static void resetGlobalBoundsTransfom();

   private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	sf::CircleShape shape;
	sf::Vector2f velocity;
	float massInverse;

	static BoundsTransform::BoundsTransformFct boundModification;
};