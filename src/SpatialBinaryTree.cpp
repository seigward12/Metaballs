#include "SpatialBinaryTree.hpp"
#include <set>

void SpatialBinaryTree::clear(const Particle*) {
	bottomBoundTree.clear();
	leftBoundTree.clear();
	rightBoundTree.clear();
	topBoundTree.clear();
}

void SpatialBinaryTree::emplace(const Particle* particle) {
	const sf::FloatRect bounds = particle->getGlobalBounds();
	bottomBoundTree.emplace(bounds.top + bounds.height, particle);
	leftBoundTree.emplace(bounds.left, particle);
	rightBoundTree.emplace(bounds.left + bounds.width, particle);
	topBoundTree.emplace(bounds.top, particle);
}
void SpatialBinaryTree::erase(const Particle* particle) {}

void SpatialBinaryTree::query(
	const sf::FloatRect& box,
	std::unordered_set<Particle*>& particlesInCollision) {
	sf::Vector2f boxCenterPosition = sf::Vector2f(box.left + (box.width / 2.f),
												  box.top + (box.height / 2.f));
	std::unordered_set<const Particle*> particlesInXRange;
	std::unordered_set<const Particle*> particlesInYRange;

	//pour toutes les particles dont le coté droit est plus grand que le côté gauche de la box,
	//jusqu'à ce qe le coté gauche de la particule soit au centre de la box
	for (ParticleMap::iterator rightIt = rightBoundTree.lower_bound(box.left);
		 rightIt != rightBoundTree.end() &&
		 rightIt->first - rightIt->second->getRadius() <= boxCenterPosition.x;
		 ++rightIt)
		particlesInXRange.emplace(rightIt->second);

	for (ParticleMap::reverse_iterator leftIt = std::make_reverse_iterator(
			 leftBoundTree.upper_bound(box.left + box.width));
		 leftIt != leftBoundTree.rend() && leftIt->first > boxCenterPosition.x;
		 ++leftIt)
		particlesInXRange.emplace(leftIt->second);

	for (ParticleMap::iterator upIt = topBoundTree.lower_bound(box.top);
		 upIt != topBoundTree.end() &&
		 upIt->first - upIt->second->getRadius() <= boxCenterPosition.y;
		 ++upIt)
		particlesInYRange.emplace(upIt->second);

	for (ParticleMap::reverse_iterator bottomIt = std::make_reverse_iterator(
			 bottomBoundTree.upper_bound(box.top + box.height));
		 bottomIt != bottomBoundTree.rend() &&
		 bottomIt->first > boxCenterPosition.y;
		 ++bottomIt)
		particlesInYRange.emplace(bottomIt->second);

	for (const Particle* particle : particlesInXRange) {
		if (particlesInYRange.contains(particle))
			particlesInCollision.emplace(particle);
	}
}

void SpatialBinaryTree::query(
	const Particle* particle,
	std::unordered_set<Particle*>& particlesInCollision) {
	query(particle->getGlobalBounds(), particlesInCollision);
}

void SpatialBinaryTree::move(const Particle* particle) {}