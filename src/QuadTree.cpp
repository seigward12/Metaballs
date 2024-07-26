#include "QuadTree.hpp"

class QuadTree::Node : public sf::Drawable {
   public:
	Node(const sf::FloatRect& boundary, int capacity);
	void insert(Particle* object);
	void query(const sf::Vector2f, std::unordered_set<Particle*>) const;
	void query(const sf::FloatRect&, std::unordered_set<Particle*>) const;
	bool isSmaller(const sf::FloatRect&) const;
	void clear();

   private:
	void subdivide();
	bool isDivided() const;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	static const int CHILD_NUMBER = 4;
	Node* parentNode = nullptr;
	int capacity;
	std::unique_ptr<Node>
		childNodes[CHILD_NUMBER];  //0: NW, 1: NE, 2: SW, 3: SE
	std::vector<Particle*> objects{};
	sf::VertexArray boundaryLines{sf::LineStrip, 5};
	const sf::FloatRect boundary;
	sf::FloatRect smallestBoundingArea;
};

QuadTree::QuadTree(const sf::FloatRect& boundary, int capacity)
	: capacity(capacity), boundary(boundary) {
	rootNode = new Node(boundary, capacity);
}

QuadTree::~QuadTree() {
	delete rootNode;
}

void QuadTree::reset() {
	delete rootNode;
	rootNode = new Node(boundary, capacity);
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*rootNode, states);
}

void QuadTree::insert(Particle* object) {
	rootNode->insert(object);
}

std::unordered_set<Particle*> QuadTree::query(Particle* particle) const {
	return query(particle->getGlobalBounds());
}

std::unordered_set<Particle*> QuadTree::query(const sf::Vector2f point) const {
	std::unordered_set<Particle*> objectsFound;
	rootNode->query(point, objectsFound);
	return objectsFound;
}

std::unordered_set<Particle*> QuadTree::query(
	const sf::FloatRect& range) const {
	std::unordered_set<Particle*> objectsFound;
	rootNode->query(range, objectsFound);
	return objectsFound;
}

QuadTree::Node::Node(const sf::FloatRect& boundary, int capacity)
	: capacity(capacity), boundary(boundary) {
	smallestBoundingArea =
		sf::FloatRect(sf::Vector2f(boundary.left + boundary.width,
								   boundary.top + boundary.height),
					  sf::Vector2f(-boundary.width, -boundary.height));
}

bool QuadTree::Node::isSmaller(const sf::FloatRect& rect) const {
	return rect.width <= boundary.width && rect.height <= boundary.height;
}

bool QuadTree::Node::isDivided() const {
	return childNodes[0] != nullptr;
}

void QuadTree::Node::query(const sf::FloatRect& range,
						   std::unordered_set<Particle*> objectsFound) const {
	if (!smallestBoundingArea.intersects(range))
		return;

	if (isDivided()) {
		for (int i = 0; i < CHILD_NUMBER; ++i)
			childNodes[i]->query(range, objectsFound);
	}
	for (Particle* object : objects) {
		if (range.intersects(object->getGlobalBounds()))
			objectsFound.insert(object);
	}
}

void QuadTree::Node::query(const sf::Vector2f point,
						   std::unordered_set<Particle*> objectsFound) const {
	if (!smallestBoundingArea.contains(point))
		return;

	if (isDivided()) {
		for (int i = 0; i < CHILD_NUMBER; ++i)
			childNodes[i]->query(point, objectsFound);
	}
	for (Particle* object : objects) {
		if (object->getGlobalBounds().contains(point))
			objectsFound.insert(object);
	}
}

void QuadTree::Node::draw(sf::RenderTarget& target,
						  sf::RenderStates states) const {
	if (objects.size() != 0 || isDivided())
		target.draw(boundaryLines);

	if (isDivided()) {
		for (int i = 0; i < CHILD_NUMBER; ++i)
			childNodes[i]->draw(target, states);
	}
}

void QuadTree::Node::insert(Particle* object) {
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
	else if (isDivided()) {
		sf::Vector2f centerPosition = object->getCenterPosition();
		int quadrantNumber = 0;
		if (centerPosition.x > boundary.left + boundary.width / 2.f)
			quadrantNumber += 1;
		if (centerPosition.y > boundary.top + boundary.height / 2.f)
			quadrantNumber += 2;
		childNodes[quadrantNumber]->insert(object);

	} else {
		objects.push_back(object);
		if (!isDivided() && objects.size() > capacity)
			subdivide();
	}
}

void QuadTree::Node::subdivide() {
	for (int i = 0; i < CHILD_NUMBER; ++i) {
		childNodes[i] = std::make_unique<Node>(
			sf::FloatRect(boundary.getPosition() +
							  sf::Vector2f(boundary.width * (i % 2),
										   boundary.height * ((i + 1) % 2)),
						  boundary.getSize() / 2.f),
			capacity);
	}

	std::vector<Particle*> objectsCopy = objects;
	objects.clear();
	while (!objectsCopy.empty()) {
		Particle* object = objectsCopy.back();
		objectsCopy.pop_back();
		insert(object);
	}
}
