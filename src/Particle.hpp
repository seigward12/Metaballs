#pragma once
#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable {
   public:
	explicit Particle(float radius);
	~Particle();

	void update(const sf::Time& dt, const sf::FloatRect& boundary);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void collideWithParticle(Particle& other,
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

	bool operator==(const Particle& other) const;
	bool operator!=(const Particle& other) const;

   private:
	sf::CircleShape shape;
	sf::Vector2f velocity;
	float massInverse;

	inline static unsigned short count = 0;
	unsigned short id;
};