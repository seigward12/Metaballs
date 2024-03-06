#include "Particle.hpp"

Particle::Particle(const float radius) {
    id = count++;
    shape = sf::CircleShape(radius);
    shape.setPosition(0, 0);
    shape.setFillColor(sf::Color::White);
    velocity = sf::Vector2f(0, 0);
}

Particle::~Particle() {
    count--;
}

void Particle::update(const sf::Time& dt, const sf::FloatRect boundary) {
    if (shape.getPosition().x < boundary.left) {
        velocity.x *= -1;
        shape.setPosition(boundary.left, shape.getPosition().y);
    } else if (shape.getPosition().x + shape.getRadius() * 2 >
               boundary.left + boundary.width) {
        velocity.x *= -1;
        shape.setPosition(
            boundary.left + boundary.width - shape.getRadius() * 2,
            shape.getPosition().y);
    }

    if (shape.getPosition().y < boundary.top) {
        velocity.y *= -1;
        shape.setPosition(shape.getPosition().x, boundary.top);
    } else if (shape.getPosition().y + shape.getRadius() * 2 >
               boundary.top + boundary.height) {
        velocity.y *= -1;
        shape.setPosition(
            shape.getPosition().x,
            boundary.top + boundary.height - shape.getRadius() * 2);
    }

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

sf::Vector2f Particle::getCenterPosition() const {
    return shape.getPosition() +
           sf::Vector2f(shape.getRadius(), shape.getRadius());
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
    return sf::FloatRect(
        shape.getPosition(),
        sf::Vector2f(shape.getRadius() * 2, shape.getRadius() * 2));
}

bool Particle::operator==(const Particle& other) const {
    return id == other.id;
}

bool Particle::operator!=(const Particle& other) const {
    return !(*this == other);
}

bool Particle::isColliding(const Particle& other) const {
    const float distanceX = shape.getPosition().x - other.shape.getPosition().x;
    const float distanceY = shape.getPosition().y - other.shape.getPosition().y;
    const float minDistance = this->getRadius() + other.getRadius();
    return (distanceX * distanceX) + (distanceY * distanceY) <=
           (minDistance * minDistance);
}

void collideWithParticle(Particle& other) {}