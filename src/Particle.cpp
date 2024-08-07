#include "Particle.hpp"
#include <cmath>

Particle::Particle(const float radius) {
	setRadius(radius);
	setPosition(sf::Vector2f(0, 0));
	shape.setFillColor(sf::Color::White);
	velocity = sf::Vector2f(0, 0);
}

Particle::~Particle() {}

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
	target.draw(shape, states);
}

void Particle::setPosition(const sf::Vector2f position) {
	shape.setPosition(position -
					  sf::Vector2f(shape.getRadius(), shape.getRadius()));
}

void Particle::setVelocity(const sf::Vector2f _velocity) {
	velocity = _velocity;
}

void Particle::setRadius(const float radius) {
	shape.setRadius(radius);
	shape.setPointCount(std::max(static_cast<int>(std::sqrt(radius) * 4.), 3));
	massInverse = 1.f / (radius * radius);
}

void Particle::setInfiniteMass(bool isInfiniteMass) {
	massInverse =
		isInfiniteMass ? 0.0f : 1 / (shape.getRadius() * shape.getRadius());
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
	return shape.getGlobalBounds();
}

bool Particle::isColliding(const Particle& other) const {
	const sf::Vector2f distance =
		getCenterPosition() - other.getCenterPosition();
	const float minDistance = this->getRadius() + other.getRadius();
	return (distance.x * distance.x) + (distance.y * distance.y) <=
		   (minDistance * minDistance);
}

bool Particle::collideWithParticle(Particle& other,
								   const float restitutionCoefficient) {
	if (!isColliding(other))
		return false;

	sf::Vector2f distance = getCenterPosition() - other.getCenterPosition();
	sf::Vector2f normal =
		distance / static_cast<float>(
					   sqrt(distance.x * distance.x + distance.y * distance.y));

	const sf::Vector2f relativeVelocity = getVelocity() - other.getVelocity();

	const float vrMinus =
		normal.x * relativeVelocity.x + normal.y * relativeVelocity.y;

	const float massInverseTotal = massInverse + other.massInverse;
	const float j = -(1 + restitutionCoefficient) * vrMinus / massInverseTotal;

	velocity = velocity + j * normal * massInverse;
	other.velocity = other.velocity - j * normal * other.massInverse;

	// avoid overlapping
	const sf::Vector2f minDistance =
		(this->getRadius() + other.getRadius()) * normal;
	const sf::Vector2f repulsion = minDistance - distance;
	shape.move(repulsion * (massInverse / massInverseTotal));
	other.shape.move(-repulsion * (other.massInverse / massInverseTotal));

	return true;
}
