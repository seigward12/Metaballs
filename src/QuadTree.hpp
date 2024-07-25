#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"

class QuadTree : public sf::Drawable {
   public:
	QuadTree(const sf::FloatRect& boundary, unsigned short capacity);
	~QuadTree();

	void reset();
	void insert(Particle* object);
	void query(Particle* particle,
			   std::unordered_set<Particle*>& objectsFound) const;
	void query(const sf::Vector2f point,
			   std::unordered_set<Particle*>& objectsFound) const;
	void query(const sf::FloatRect& range,
			   std::unordered_set<Particle*>& objectsFound) const;

   private:
	QuadTree(const sf::FloatRect& boundary, unsigned short capacity, QuadTree*);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void subdivide();

	QuadTree* parentNode = nullptr;
	unsigned short capacity;
	bool divided = false;
	std::unique_ptr<QuadTree> northWest = nullptr, northEast = nullptr,
							  southWest = nullptr, southEast = nullptr;
	std::vector<Particle*> objects{};
	sf::VertexArray boundaryLines{sf::LineStrip, 5};
	const sf::FloatRect boundary;
	sf::FloatRect smallestBoundingArea;

	// static std::unordered_map<Particle*, QuadTree*> objectsNode;
};
