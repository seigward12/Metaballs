#pragma once
#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable {
   public:
    explicit Particle(float radius);

    void update(const sf::Time& dt, sf::FloatRect boundary);

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setPosition(sf::Vector2f position);

    void setVelocity(sf::Vector2f velocity);

    void setRadius(float radius);

    void setColor(sf::Color color);

    sf::Vector2f getPosition() const;

    sf::Vector2f getVelocity() const;

    float getRadius() const;

    sf::Color getColor() const;

    sf::CircleShape getShape() const;

    sf::FloatRect getGlobalBounds() const;

   private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
};