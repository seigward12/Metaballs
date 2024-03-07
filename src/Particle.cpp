#include "Particle.hpp"

#include <iostream>  //TODO remove

Particle::Particle(const float radius) {
    id = count++;
    shape = sf::CircleShape(radius);
    setRadius(radius);
    setPosition(sf::Vector2f(0, 0));
    shape.setFillColor(sf::Color::White);
    velocity = sf::Vector2f(0, 0);
}

Particle::~Particle() {
    count--;
}

void Particle::update(const sf::Time& dt, const sf::FloatRect& boundary) {
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
    massInverse = 1 / (radius * radius);
}

void Particle::setInfiniteMass(bool isInfiniteMass) {
    massInverse =
        isInfiniteMass ? 0.0 : 1 / (shape.getRadius() * shape.getRadius());
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
    const sf::Vector2f distance =
        getCenterPosition() - other.getCenterPosition();
    const float minDistance = this->getRadius() + other.getRadius();
    return (distance.x * distance.x) + (distance.y * distance.y) <=
           (minDistance * minDistance);
}

void Particle::collideWithParticle(const Particle& other) {
    if (isColliding(other)) {
        sf::Vector2f normale = getCenterPosition() - other.getCenterPosition();
        normale = normale / sqrt(normale.x * normale.x + normale.y * normale.y);
        std::cout << "collision normale: " << normale.x << " " << normale.y
                  << std::endl;

        const sf::Vector2f relativeVelocity =
            getVelocity() - other.getVelocity();
        std::cout << "collision relative velocity: " << relativeVelocity.x
                  << " " << relativeVelocity.y << std::endl;

        const float vrMinus =
            normale.x * relativeVelocity.x + normale.y * relativeVelocity.y;
        std::cout << "collision vrMinus: " << vrMinus << std::endl;

        const float j =
            -2 * vrMinus *
            (1 / (massInverse + other.massInverse));  // 2 parce que coeficient
        // de restitution = 1
        std::cout << "collision j: " << j << std::endl;

        velocity = velocity + j * normale * massInverse;
        std::cout << "collision velocity: " << velocity.x << " " << velocity.y
                  << std::endl
                  << std::endl;
    }
}