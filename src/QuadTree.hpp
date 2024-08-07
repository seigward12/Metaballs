#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "Particle.hpp"

class QuadTree : public sf::Drawable {
   public:
	QuadTree(const sf::FloatRect& boundary, unsigned int capacity);
	~QuadTree();

	void clear();
	void insert(Particle* object);
	void update(Particle* object);
	std::unordered_set<Particle*> query(Particle* particle) const;
	std::unordered_set<Particle*> query(const sf::Vector2f point) const;
	std::unordered_set<Particle*> query(const sf::FloatRect& range) const;

	std::vector<sf::FloatRect> findAllBounds() const;

   private:
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	const sf::FloatRect boundary;
	unsigned int capacity;

	class Node;
	friend class Node;
	std::unique_ptr<Node> rootNode;
	std::unordered_map<Particle*, Node*> objectsNode;
};
