#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

const unsigned short DIVISION_SIZE = 4;

template <class DataType>
class QuadTree2 : public sf::Drawable {
   public:
    QuadTree2(const sf::FloatRect& boundary, unsigned short capacity);
    ~QuadTree2();
    void reset();
    bool insert(DataType* object);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    void query(sf::FloatRect range, std::vector<DataType*>& objectsFound);

   private:
    unsigned short capacity;
    bool divided = false;
    std::unique_ptr<QuadTree2> northWest, northEast, southWest,
        southEast = nullptr;
    std::vector<DataType*> objects{};
    sf::VertexArray lines{sf::LineStrip, DIVISION_SIZE};
    const sf::FloatRect boundary{};

    void subdivide();
};

template <class DataType>
QuadTree2<DataType>::QuadTree2(const sf::FloatRect& boundary,
                               unsigned short capacity)
    : capacity(capacity), boundary(boundary) {
    lines[0].position = boundary.getPosition();
    lines[1].position =
        boundary.getPosition() + sf::Vector2f(boundary.width, 0);
    lines[2].position =
        boundary.getPosition() + sf::Vector2f(boundary.width, boundary.height);
    lines[3].position =
        boundary.getPosition() + sf::Vector2f(0, boundary.height);

    const sf::Color color = sf::Color::Yellow;
    for (int i = 0; i < lines.getVertexCount(); ++i) {
        lines[i].color = color;
    }
}

template <class DataType>
QuadTree2<DataType>::~QuadTree2() {
    reset();
}

template <class DataType>
void QuadTree2<DataType>::reset() {
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
void QuadTree2<DataType>::draw(sf::RenderTarget& target,
                               sf::RenderStates states) const {
    target.draw(lines);
    if (divided) {
        target.draw(*northWest, states);
        target.draw(*northEast, states);
        target.draw(*southEast, states);
        target.draw(*southWest, states);
    }
}

template <class DataType>
void QuadTree2<DataType>::subdivide() {
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

    northWest = std::make_unique<QuadTree2<DataType>>(nw, capacity);
    northEast = std::make_unique<QuadTree2<DataType>>(ne, capacity);
    southWest = std::make_unique<QuadTree2<DataType>>(sw, capacity);
    southEast = std::make_unique<QuadTree2<DataType>>(se, capacity);
}

template <class DataType>
bool QuadTree2<DataType>::insert(DataType* object) {
    if (boundary.intersects(object->getGlobalBounds())) {
        if (objects.size() < capacity) {
            objects.push_back(object);
            return true;
        } else {
            if (!divided)
                subdivide();

            if (northWest->insert(object) || northEast->insert(object) ||
                southWest->insert(object) || southEast->insert(object))
                return true;
        }
    }
    return false;
}

template <class DataType>
void QuadTree2<DataType>::query(sf::FloatRect range,
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