#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"

typedef std::multimap<float, const Particle*> ParticleMap;

class SpatialBinaryTree {
   public:
	SpatialBinaryTree() = default;
	~SpatialBinaryTree() = default;
	void emplace(const Particle*);
	void erase(const Particle*);
	void query(const sf::FloatRect&, std::unordered_set<Particle*>&);
	void query(const Particle*, std::unordered_set<Particle*>&);
	void move(const Particle*);

   private:
	ParticleMap bottomBoundTree;
	ParticleMap leftBoundTree;
	ParticleMap rightBoundTree;
	ParticleMap topBoundTree;

	// struct ParticlePositionInTrees {
	// 	ParticleMap::iterator bottomIt;
	// 	ParticleMap::iterator leftIt;
	// 	ParticleMap::iterator rightIt;
	// 	ParticleMap::iterator topIt;
	// };
	// std::unordered_map<const Particle*, ParticlePositionInTrees> leftBoundTree;
};
