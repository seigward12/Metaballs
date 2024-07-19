#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <unordered_set>
constexpr unsigned short DIVISION_SIZE = 4;
constexpr unsigned short LINES_PER_NODE = 3;

template <class DataType>
class QuadTree : public sf::Drawable {
   public:
	QuadTree(const sf::FloatRect& boundary, unsigned short capacity);
	~QuadTree();

	void reset();
	bool insert(DataType* object);
	void query(const sf::FloatRect& range,
			   std::unordered_set<DataType*>& objectsFound);
	bool contains(const sf::Vector2f&);

   private:
	QuadTree(const sf::FloatRect& boundary, unsigned short capacity, QuadTree*);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void subdivide();

	QuadTree<DataType>* parentNode = nullptr;
	unsigned short capacity;
	bool divided = false;
	std::unique_ptr<QuadTree> northWest = nullptr, northEast = nullptr,
							  southWest = nullptr, southEast = nullptr;
	std::vector<DataType*> objects{};
	sf::VertexArray boundaryLines{sf::LineStrip, LINES_PER_NODE};
	const sf::FloatRect boundary{};

	static std::unordered_map<DataType*, QuadTree<DataType>*> objectsNode;
};

template <class DataType>
std::unordered_map<DataType*, QuadTree<DataType>*>
	QuadTree<DataType>::objectsNode{};

template <class DataType>
QuadTree<DataType>::QuadTree(const sf::FloatRect& boundary,
							 unsigned short capacity)
	: capacity(capacity), boundary(boundary) {
	boundaryLines[0].position =
		boundary.getPosition() + sf::Vector2f(0.f, boundary.height);
	boundaryLines[1].position = boundary.getPosition();
	boundaryLines[2].position =
		boundary.getPosition() + sf::Vector2f(boundary.width, 0.f);

	const sf::Color color = sf::Color::Yellow;
	for (int i = 0; i < LINES_PER_NODE; ++i) {
		boundaryLines[i].color = color;
	}
}

template <class DataType>
QuadTree<DataType>::QuadTree(const sf::FloatRect& boundary,
							 unsigned short capacity,
							 QuadTree* parentNode)
	: QuadTree(boundary, capacity) {
	parentNode = parentNode;
}

template <class DataType>
QuadTree<DataType>::~QuadTree() {
	reset();
}

template <class DataType>
void QuadTree<DataType>::reset() {
	divided = false;
	objects.clear();
	boundaryLines[1].position = boundary.getPosition();
}

template <class DataType>
void QuadTree<DataType>::draw(sf::RenderTarget& target,
							  sf::RenderStates states) const {
	target.draw(boundaryLines);
	if (divided) {
		target.draw(*northWest, states);
		target.draw(*northEast, states);
		target.draw(*southEast, states);
		target.draw(*southWest, states);
	}
}

template <class DataType>
bool QuadTree<DataType>::contains(const sf::Vector2f& position) {
	return boundary.top < position.y && boundary.left < position.x &&
		   boundary.top + boundary.height > position.y &&
		   boundary.left + boundary.width > position.x;
}

template <class DataType>
void QuadTree<DataType>::subdivide() {
	divided = true;
	const sf::Vector2f dividedSize(boundary.width / 2, boundary.height / 2);

	const sf::FloatRect nw(boundary.getPosition(), dividedSize);
	const sf::FloatRect ne(nw.getPosition() + sf::Vector2f(dividedSize.x, 0),
						   nw.getSize());
	const sf::FloatRect se(
		nw.getPosition() + sf::Vector2f(dividedSize.x, dividedSize.y),
		nw.getSize());
	const sf::FloatRect sw(nw.getPosition() + sf::Vector2f(0, dividedSize.y),
						   nw.getSize());

	northWest = std::unique_ptr<QuadTree>(new QuadTree(nw, capacity, this));
	northEast = std::unique_ptr<QuadTree>(new QuadTree(ne, capacity, this));
	southWest = std::unique_ptr<QuadTree>(new QuadTree(sw, capacity, this));
	southEast = std::unique_ptr<QuadTree>(new QuadTree(se, capacity, this));

	while (!objects.empty()) {
		DataType* object = objects.back();
		objects.pop_back();
		insert(object);
	}

	boundaryLines[1].position += sf::Vector2f(boundary.width, boundary.height);
}

template <class DataType>
bool QuadTree<DataType>::insert(DataType* object) {
	// auto objectNode = objectsNode.find(object);
	// if (objectNode != objectsNode.end()) {
	// }
	// if (objectsNode.fin)

	sf::Vector2f position = object->getCenterPosition();
	if (!contains(position))
		return false;

	if (divided) {
		return northWest->insert(object) || northEast->insert(object) ||
			   southWest->insert(object) || southEast->insert(object);
	} else {
		objects.push_back(object);
		if (objects.size() > capacity)
			subdivide();
		return true;
	}
}

template <class DataType>
void QuadTree<DataType>::query(const sf::FloatRect& range,
							   std::unordered_set<DataType*>& objectsFound) {
	if (!boundary.intersects(range))
		return;

	if (divided) {
		northWest->query(range, objectsFound);
		northEast->query(range, objectsFound);
		southWest->query(range, objectsFound);
		southEast->query(range, objectsFound);
	} else {
		for (DataType* object : objects) {
			if (range.intersects(object->getGlobalBounds()))
				objectsFound.insert(object);
		}
	}
}