#include "SpatialBinaryTree.hpp"
#include <set>

void SpatialBinaryTree::clear() {
	bottomBoundTree.clear();
	leftBoundTree.clear();
	rightBoundTree.clear();
	topBoundTree.clear();
	treeIterators.clear();
}

void SpatialBinaryTree::emplace(const Particle* particle) {
	if (particle == nullptr)
		return;
	const sf::FloatRect bounds = particle->getGlobalBounds();

	treeIterators.emplace(
		particle,
		ParticlePositionInTrees{
			bottomBoundTree.emplace(bounds.top + bounds.height, particle),
			leftBoundTree.emplace(bounds.left, particle),
			rightBoundTree.emplace(bounds.left + bounds.width, particle),
			topBoundTree.emplace(bounds.top, particle)});
}
void SpatialBinaryTree::erase(const Particle* particle) {
	const auto& particleIterators = treeIterators.find(particle);
	if (particleIterators == treeIterators.end())
		return;
	bottomBoundTree.erase(particleIterators->second.bottomIt);
	leftBoundTree.erase(particleIterators->second.leftIt);
	rightBoundTree.erase(particleIterators->second.rightIt);
	topBoundTree.erase(particleIterators->second.topIt);
	treeIterators.erase(particle);
}

void SpatialBinaryTree::move(const Particle* particle) {
	const auto& particleNode = treeIterators.find(particle);
	if (particleNode == treeIterators.end())
		return;
	ParticlePositionInTrees& particleIterators = particleNode->second;
	const sf::FloatRect& newBounds = particle->getGlobalBounds();

	if (newBounds.top != particleIterators.topIt->first) {
		auto bottomNode = bottomBoundTree.extract(particleIterators.bottomIt);
		bottomNode.key() = newBounds.top + newBounds.height;
		particleIterators.bottomIt =
			bottomBoundTree.insert(std::move(bottomNode));
		auto topNode = topBoundTree.extract(particleIterators.topIt);
		topNode.key() = newBounds.top;
		particleIterators.topIt = topBoundTree.insert(std::move(topNode));
	}

	if (newBounds.left != particleIterators.leftIt->first) {
		auto leftNode = leftBoundTree.extract(particleIterators.leftIt);
		leftNode.key() = newBounds.left;
		particleIterators.leftIt = leftBoundTree.insert(std::move(leftNode));
		auto rightNode = rightBoundTree.extract(particleIterators.rightIt);
		rightNode.key() = newBounds.left + newBounds.width;
		particleIterators.rightIt = rightBoundTree.insert(std::move(rightNode));
	}
}

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
