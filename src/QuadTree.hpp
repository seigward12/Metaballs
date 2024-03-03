#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

const unsigned short DIVISION_SIZE = 4;
const unsigned short LINES_PER_NODE = 3;

template <class DataType>
class QuadTree : public sf::Drawable {
   public:
    QuadTree(const sf::FloatRect& boundary, unsigned short capacity);
    ~QuadTree();
    void reset();
    bool insert(DataType* object);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void query(sf::FloatRect range, std::vector<DataType*>& objectsFound);
    // void queryCollision(const DataType* object,
    //                     std::vector<DataType*>& objectsFound);

   private:
    unsigned short capacity;
    bool divided = false;
    std::unique_ptr<QuadTree> northWest, northEast, southWest,
        southEast = nullptr;
    std::vector<DataType*> objects{};
    sf::VertexArray parentLines{sf::LineStrip, LINES_PER_NODE};
    sf::VertexArray childLines{sf::LineStrip, LINES_PER_NODE};
    const sf::FloatRect boundary{};

    void subdivide();
};

template <class DataType>
QuadTree<DataType>::QuadTree(const sf::FloatRect& boundary,
                             unsigned short capacity)
    : capacity(capacity), boundary(boundary) {
    parentLines[0].position =
        boundary.getPosition() + sf::Vector2f(0.f, boundary.height);
    parentLines[1].position = boundary.getPosition();
    parentLines[2].position =
        boundary.getPosition() + sf::Vector2f(boundary.width, 0.f);
    childLines[0].position = parentLines[2].position;
    childLines[1].position =
        boundary.getPosition() + sf::Vector2f(boundary.width, boundary.height);
    childLines[2].position =
        boundary.getPosition() + sf::Vector2f(0.f, boundary.height);

    const sf::Color color = sf::Color::Yellow;
    for (int i = 0; i < LINES_PER_NODE; ++i) {
        parentLines[i].color = color;
        childLines[i].color = color;
    }
}

template <class DataType>
QuadTree<DataType>::~QuadTree() {
    reset();
}

template <class DataType>
void QuadTree<DataType>::reset() {
    // if (divided) {
    //     delete northWest.re;
    //     delete northEast;
    //     delete southWest;
    //     delete southEast;
    // }
    divided = false;
    objects.clear();
}

template <class DataType>
void QuadTree<DataType>::draw(sf::RenderTarget& target,
                              sf::RenderStates states) const {
    if (divided) {
        target.draw(parentLines);
        target.draw(*northWest, states);
        target.draw(*northEast, states);
        target.draw(*southEast, states);
        target.draw(*southWest, states);
    } else {
        target.draw(childLines);
    }
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

    northWest = std::make_unique<QuadTree<DataType>>(nw, capacity);
    northEast = std::make_unique<QuadTree<DataType>>(ne, capacity);
    southWest = std::make_unique<QuadTree<DataType>>(sw, capacity);
    southEast = std::make_unique<QuadTree<DataType>>(se, capacity);
}

template <class DataType>
bool QuadTree<DataType>::insert(DataType* object) {
    if (!boundary.intersects(object->getGlobalBounds()))
        return false;

    if (objects.size() < capacity) {
        objects.push_back(object);
        return true;
    } else {
        if (!divided)
            subdivide();

        return northWest->insert(object) || northEast->insert(object) ||
               southWest->insert(object) || southEast->insert(object);
    }
}

template <class DataType>
void QuadTree<DataType>::query(sf::FloatRect range,
                               std::vector<DataType*>& objectsFound) {
    if (!boundary.intersects(range))
        return;

    if (divided) {
        northWest->query(range, objectsFound);
        northEast->query(range, objectsFound);
        southWest->query(range, objectsFound);
        southEast->query(range, objectsFound);
    }
    for (DataType* object : objects) {
        if (range.intersects(object->getGlobalBounds())) {
            objectsFound.push_back(object);
        }
    }
}