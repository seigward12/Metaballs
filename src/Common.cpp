#include "Common.hpp"

namespace Collision {
bool CircleShapeCollision(const sf::CircleShape& A, const sf::CircleShape& B) {
    const float distanceX = A.getPosition().x - B.getPosition().x;
    const float distanceY = A.getPosition().y - B.getPosition().y;
    const float minDistance = A.getRadius() + B.getRadius();
    return (distanceX * distanceX) + (distanceY * distanceY) <=
           (minDistance * minDistance);
}

bool ParticleCollision(const Particle& A, const Particle& B) {
    return CircleShapeCollision(A.getShape(), B.getShape());
}

}  // namespace Collision
