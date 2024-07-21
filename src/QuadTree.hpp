#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"
constexpr unsigned short DIVISION_SIZE = 4;
constexpr unsigned short LINES_PER_NODE = 3;

class QuadTree : public sf::Drawable {
   public:
	QuadTree(const sf::FloatRect& boundary, unsigned short capacity);
	~QuadTree();

	void reset();
	bool insert(Particle* object);
	void query(const sf::FloatRect& range,
			   std::unordered_set<Particle*>& objectsFound);
	bool contains(const sf::Vector2f&);

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
	sf::VertexArray boundaryLines{sf::LineStrip, LINES_PER_NODE};
	const sf::FloatRect boundary{};

	// static std::unordered_map<Particle*, QuadTree*> objectsNode;
};
