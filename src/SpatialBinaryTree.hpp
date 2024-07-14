#pragma once
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"

//TODO essayer avec un map de vector2f left/top et right/bottom et prédicat x < x &&

class SpatialBinaryTree {
   public:
	SpatialBinaryTree() = default;
	~SpatialBinaryTree() = default;
	void clear(const Particle*);
	void emplace(const Particle*);
	void erase(const Particle*);
	void move(const Particle*);
	void query(const sf::FloatRect&, std::unordered_set<Particle*>&);
	void query(const Particle*, std::unordered_set<Particle*>&);

   private:
	using ParticleMap = std::multimap<float, const Particle*>;
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
	// std::unordered_map<const Particle*, ParticlePositionInTrees> treeIterators;
};
