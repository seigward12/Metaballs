#include "QuadTree.hpp"
#include <algorithm>

class QuadTree::Node : public sf::Drawable {
   public:
	Node(const sf::FloatRect& boundary,
		 unsigned int capacity,
		 Node* parentNode);
	void insert(Particle* object, QuadTree*);
	void update(Particle* object, QuadTree*);
	void query(const sf::Vector2f, std::unordered_set<Particle*>&) const;
	void query(const sf::FloatRect&, std::unordered_set<Particle*>&) const;
	bool isSmaller(const sf::FloatRect&) const;

	std::unordered_set<Particle*> objects;

   private:
	void subdivide(QuadTree* quadTree);
	bool isDivided() const;
	bool isEmpty() const;
	void updateSmallestBoundary();
	int findEnglobingQuadrant(const sf::Vector2f& point);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	static const int CHILD_NUMBER = 4;
	Node* parentNode = nullptr;
	unsigned int capacity;
	std::unique_ptr<Node>
		childNodes[CHILD_NUMBER];  //0: NW, 1: NE, 2: SW, 3: SE
	sf::VertexArray boundaryLines{sf::LineStrip, 5};
	const sf::FloatRect boundary;
	sf::FloatRect smallestBoundingArea;
};

QuadTree::Node::Node(const sf::FloatRect& boundary,
					 unsigned int capacity,
					 Node* parentNode)
	: capacity(capacity), boundary(boundary), parentNode(parentNode) {
	smallestBoundingArea =
		sf::FloatRect(boundary.getPosition() + boundary.getSize(),
					  boundary.getSize() * (-1.f));
}

bool QuadTree::Node::isSmaller(const sf::FloatRect& rect) const {
	return boundary.width <= rect.width || boundary.height <= rect.height;
}

bool QuadTree::Node::isDivided() const {
	return childNodes[0] != nullptr;
}

bool QuadTree::Node::isEmpty() const {
	return !(objects.size() > 0 || isDivided());
}

void QuadTree::Node::query(const sf::FloatRect& range,
						   std::unordered_set<Particle*>& objectsFound) const {
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
						   std::unordered_set<Particle*>& objectsFound) const {
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

void QuadTree::Node::insert(Particle* object, QuadTree* quadTree) {
	if (isDivided()) {
		if (isSmaller(object->getGlobalBounds())) {
			objects.insert(object);
			quadTree->objectsNode.emplace(object, this);
			updateSmallestBoundary();
		} else {
			childNodes[findEnglobingQuadrant(object->getCenterPosition())]
				->insert(object, quadTree);
		}
	} else {
		objects.insert(object);
		if (objects.size() > capacity)
			subdivide(quadTree);
		else {
			quadTree->objectsNode.emplace(object, this);
			updateSmallestBoundary();
		}
	}
}

void QuadTree::Node::update(Particle* object, QuadTree* quadTree) {
	if (boundary.contains(object->getCenterPosition()) ||
		parentNode == nullptr) {
		insert(object, quadTree);
	} else {
		parentNode->update(object, quadTree);
	}
}

void QuadTree::Node::subdivide(QuadTree* quadTree) {
	const sf::Vector2f dividedSize = boundary.getSize() / 2.f;
	const sf::FloatRect nw(boundary.getPosition(), dividedSize);
	sf::FloatRect ne = nw;
	ne.left += dividedSize.x;
	sf::FloatRect sw = nw;
	sw.top += dividedSize.y;
	sf::FloatRect se = sw;
	se.left += dividedSize.x;
	childNodes[0] = std::make_unique<Node>(nw, capacity, this);
	childNodes[1] = std::make_unique<Node>(ne, capacity, this);
	childNodes[2] = std::make_unique<Node>(sw, capacity, this);
	childNodes[3] = std::make_unique<Node>(se, capacity, this);

	for (auto it = objects.begin(); it != objects.end();) {
		if (!isSmaller((*it)->getGlobalBounds())) {
			++it;
		} else {
			Particle* particle = *it;
			it = objects.erase(it);	 //it is now potentially end
			childNodes[findEnglobingQuadrant(particle->getCenterPosition())]
				->insert(particle, quadTree);
		}
	}
}

int QuadTree::Node::findEnglobingQuadrant(const sf::Vector2f& point) {
	int quadrantNumber = 0;
	if (point.x > boundary.left + boundary.width / 2.f)
		quadrantNumber += 1;
	if (point.y > boundary.top + boundary.height / 2.f)
		quadrantNumber += 2;
	return quadrantNumber;
}

void QuadTree::Node::updateSmallestBoundary() {
	float minX = boundary.left + boundary.width,
		  minY = boundary.top + boundary.height, maxX = boundary.left,
		  maxY = boundary.top;
	bool hasAABBChanged = false;

	if (isDivided()) {
		for (int i = 0; i < CHILD_NUMBER; ++i) {
			const sf::FloatRect childBounds =
				childNodes[i]->smallestBoundingArea;
			minX = std::min(minX, childBounds.left);
			minY = std::min(minY, childBounds.top);
			maxX = std::max(maxX, childBounds.left + childBounds.width);
			maxY = std::max(maxY, childBounds.top + childBounds.height);
		}
	}
	for (Particle* object : objects) {
		const sf::FloatRect objectBounds = object->getGlobalBounds();
		minX = std::min(minX, objectBounds.left);
		minY = std::min(minY, objectBounds.top);
		maxX = std::max(maxX, objectBounds.left + objectBounds.width);
		maxY = std::max(maxY, objectBounds.top + objectBounds.height);
	}

	smallestBoundingArea = sf::FloatRect(
		sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));

	if (boundaryLines[0].position != smallestBoundingArea.getPosition()) {
		hasAABBChanged = true;
		boundaryLines[0].position = smallestBoundingArea.getPosition();
		boundaryLines[1].position.y = minY;
		boundaryLines[3].position.x = minX;
		boundaryLines[4].position = smallestBoundingArea.getPosition();
	}
	if (boundaryLines[2].position != sf::Vector2f(maxX, maxY)) {
		hasAABBChanged = true;
		boundaryLines[2].position = sf::Vector2f(maxX, maxY);
		boundaryLines[1].position.x = maxX;
		boundaryLines[3].position.y = maxY;
	}

	if (hasAABBChanged && parentNode != nullptr) {
		parentNode->updateSmallestBoundary();
	}
}

QuadTree::QuadTree(const sf::FloatRect& boundary, unsigned int capacity)
	: capacity(capacity), boundary(boundary) {
	rootNode = std::make_unique<Node>(boundary, capacity, nullptr);
}

void QuadTree::clear() {
	rootNode = std::make_unique<Node>(boundary, capacity, nullptr);
}

void QuadTree::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*rootNode, states);
}

void QuadTree::insert(Particle* object) {
	rootNode->insert(object, this);
}

void QuadTree::update(Particle* object) {
	const auto& objectNode = objectsNode.find(object);
	if (objectNode != objectsNode.end()) {
		Node* node = objectNode->second;
		objectsNode.erase(object);
		node->objects.erase(object);
		node->update(object, this);
	}
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
