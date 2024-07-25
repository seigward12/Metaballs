#include "QuadTree.hpp"

QuadTree::QuadTree(const sf::FloatRect& boundary, unsigned short capacity)
	: capacity(capacity), boundary(boundary) {
	smallestBoundingArea =
		sf::FloatRect(sf::Vector2f(boundary.left + boundary.width,
								   boundary.top + boundary.height),
					  sf::Vector2f(-boundary.width, -boundary.height));
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
	smallestBoundingArea =
		sf::FloatRect(sf::Vector2f(boundary.left + boundary.width,
								   boundary.top + boundary.height),
					  sf::Vector2f(-boundary.width, -boundary.height));
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (objects.size() != 0 || divided)
		target.draw(boundaryLines);

	if (divided) {
		target.draw(*northWest, states);
		target.draw(*northEast, states);
		target.draw(*southEast, states);
		target.draw(*southWest, states);
	}
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

	std::vector<Particle*> objectsCopy = objects;
	objects.clear();
	while (!objectsCopy.empty()) {
		Particle* object = objectsCopy.back();
		objectsCopy.pop_back();
		insert(object);
	}
}

void QuadTree::insert(Particle* object) {
	const sf::FloatRect& objectBounds = object->getGlobalBounds();
	float minX = std::min(smallestBoundingArea.left, objectBounds.left);
	float minY = std::min(smallestBoundingArea.top, objectBounds.top);
	float maxX =
		std::max(smallestBoundingArea.left + smallestBoundingArea.width,
				 objectBounds.left + objectBounds.width);
	float maxY =
		std::max(smallestBoundingArea.top + smallestBoundingArea.height,
				 objectBounds.top + objectBounds.height);
	smallestBoundingArea.left = minX;
	smallestBoundingArea.top = minY;
	smallestBoundingArea.width = maxX - smallestBoundingArea.left;
	smallestBoundingArea.height = maxY - smallestBoundingArea.top;
	if (boundaryLines[0].position != smallestBoundingArea.getPosition()) {
		boundaryLines[0].position = smallestBoundingArea.getPosition();
		boundaryLines[1].position.y = minY;
		boundaryLines[3].position.x = minX;
		boundaryLines[4].position = smallestBoundingArea.getPosition();
	}
	if (boundaryLines[2].position != sf::Vector2f(maxX, maxY)) {
		boundaryLines[2].position = sf::Vector2f(maxX, maxY);
		boundaryLines[1].position.x = maxX;
		boundaryLines[3].position.y = maxY;
	}

	if (boundary.height < objectBounds.height ||
		boundary.width < objectBounds.width)
		objects.push_back(object);
	else if (divided) {
		sf::Vector2f centerPosition = object->getCenterPosition();
		if (centerPosition.x <= boundary.left + boundary.width / 2.f) {
			if (centerPosition.y <= boundary.top + boundary.height / 2.f)
				northWest->insert(object);
			else
				southWest->insert(object);

		} else {
			if (centerPosition.y <= boundary.top + boundary.height / 2.f)
				northEast->insert(object);
			else
				southEast->insert(object);
		}
	} else {
		objects.push_back(object);
		if (!divided && objects.size() > capacity)
			subdivide();
	}
}

void QuadTree::query(Particle* particle,
					 std::unordered_set<Particle*>& objectsFound) const {
	query(particle->getGlobalBounds(), objectsFound);
}

void QuadTree::query(const sf::Vector2f point,
					 std::unordered_set<Particle*>& objectsFound) const {
	if (!smallestBoundingArea.contains(point))
		return;
	if (divided) {
		northWest->query(point, objectsFound);
		northEast->query(point, objectsFound);
		southWest->query(point, objectsFound);
		southEast->query(point, objectsFound);
	}
	for (Particle* object : objects) {
		if (object->getGlobalBounds().contains(point))
			objectsFound.insert(object);
	}
}

void QuadTree::query(const sf::FloatRect& range,
					 std::unordered_set<Particle*>& objectsFound) const {
	if (!smallestBoundingArea.intersects(range))
		return;

	if (divided) {
		northWest->query(range, objectsFound);
		northEast->query(range, objectsFound);
		southWest->query(range, objectsFound);
		southEast->query(range, objectsFound);
	}
	for (Particle* object : objects) {
		if (range.intersects(object->getGlobalBounds()))
			objectsFound.insert(object);
	}
}