#include "Particle.hpp"

Particle::Particle(const float radius) {
    id = count++;
    shape.setPosition(0, 0);
    shape.setRadius(radius);
    shape.setFillColor(sf::Color::White);
    velocity = sf::Vector2f(0, 0);
}

Particle::~Particle() {
    count--;
}

void Particle::update(const sf::Time& dt, const sf::FloatRect boundary) {
    if (shape.getPosition().x < boundary.left ||
        shape.getPosition().x + shape.getRadius() * 2 >
            boundary.left + boundary.width)
        velocity.x *= -1;

    if (shape.getPosition().y < boundary.top ||
        shape.getPosition().y + shape.getRadius() * 2 >
            boundary.top + boundary.height)
        velocity.y *= -1;

    shape.move(velocity * dt.asSeconds());
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape);
}

void Particle::setPosition(const sf::Vector2f position) {
    shape.setPosition(position -
                      sf::Vector2f(shape.getRadius(), shape.getRadius()));
}

void Particle::setVelocity(const sf::Vector2f velocity) {
    this->velocity = velocity;
}

void Particle::setRadius(const float radius) {
    shape.setRadius(radius);
}

void Particle::setColor(const sf::Color color) {
    shape.setFillColor(color);
}

sf::Vector2f Particle::getPosition() const {
    return shape.getPosition();
}

sf::Vector2f Particle::getVelocity() const {
    return velocity;
}

float Particle::getRadius() const {
    return shape.getRadius();
}

sf::Color Particle::getColor() const {
    return shape.getFillColor();
}

sf::CircleShape Particle::getShape() const {
    return shape;
}

sf::FloatRect Particle::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

bool Particle::operator==(const Particle& other) const {
    return id == other.id;
}

bool Particle::operator!=(const Particle& other) const {
    return !(*this == other);
}