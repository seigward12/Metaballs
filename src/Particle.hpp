#pragma once
#include <SFML/Graphics.hpp>

class Particle : public sf::Drawable {
   public:
    explicit Particle(float radius);
    ~Particle();

    void update(const sf::Time& dt, sf::FloatRect boundary);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setPosition(sf::Vector2f position);
    void setVelocity(sf::Vector2f velocity);
    void setRadius(float radius);
    void setColor(sf::Color color);

    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getVelocity() const;
    float getRadius() const;
    sf::Color getColor() const;
    sf::CircleShape getShape() const;

    bool operator==(const Particle& other) const;
    bool operator!=(const Particle& other) const;

   private:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    inline static unsigned short count = 0;
    unsigned short id;
};