#include "QuadTree.hpp"

QuadTree::QuadTree(const sf::FloatRect& boundary, unsigned short capacity)
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

QuadTree::QuadTree(const sf::FloatRect& boundary,
				   unsigned short capacity,
				   QuadTree* parentNode)
	: QuadTree(boundary, capacity) {
	parentNode = parentNode;
}

QuadTree::~QuadTree() {
	reset();
}

void QuadTree::reset() {
	divided = false;
	objects.clear();
	boundaryLines[1].position = boundary.getPosition();
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(boundaryLines);
	if (divided) {
		target.draw(*northWest, states);
		target.draw(*northEast, states);
		target.draw(*southEast, states);
		target.draw(*southWest, states);
	}
}

bool QuadTree::contains(const sf::Vector2f& position) {
	return boundary.top < position.y && boundary.left < position.x &&
		   boundary.top + boundary.height > position.y &&
		   boundary.left + boundary.width > position.x;
}

void QuadTree::subdivide() {
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
		Particle* object = objects.back();
		objects.pop_back();
		insert(object);
	}

	boundaryLines[1].position += sf::Vector2f(boundary.width, boundary.height);
}

bool QuadTree::insert(Particle* object) {
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

void QuadTree::query(const sf::FloatRect& range,
					 std::unordered_set<Particle*>& objectsFound) {
	if (!boundary.intersects(range))
		return;

	if (divided) {
		northWest->query(range, objectsFound);
		northEast->query(range, objectsFound);
		southWest->query(range, objectsFound);
		southEast->query(range, objectsFound);
	} else {
		for (Particle* object : objects) {
			if (range.intersects(object->getGlobalBounds()))
				objectsFound.insert(object);
		}
	}
}