#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"

//TODO essayer avec un map de vector2f left/top et right/bottom et prédicat x < x &&
//TODO make tree be the only owner of particles, and offer a method to give iterator

class SpatialBinaryTree : public sf::Drawable {
   public:
	SpatialBinaryTree() = default;
	~SpatialBinaryTree() = default;
	void clear();
	void emplace(const Particle*);
	void erase(const Particle*);
	void move(const Particle*);
	void query(const sf::FloatRect&, std::unordered_set<Particle*>&);
	void query(const Particle*, std::unordered_set<Particle*>&);

   private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	using ParticleMap = std::multimap<float, const Particle*>;
	ParticleMap bottomBoundTree;
	ParticleMap leftBoundTree;
	ParticleMap rightBoundTree;
	ParticleMap topBoundTree;

	struct ParticlePositionInTrees {
		ParticleMap::iterator bottomIt;
		ParticleMap::iterator leftIt;
		ParticleMap::iterator rightIt;
		ParticleMap::iterator topIt;
	};
	std::unordered_map<const Particle*, ParticlePositionInTrees> treeIterators;
};
