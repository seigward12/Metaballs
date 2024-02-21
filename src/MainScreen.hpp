#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Button.hpp"
#include "Particle.hpp"
#include "QuadTree.hpp"
#include "State.hpp"
#include "StateManager.hpp"
#include "TextBox.hpp"

const sf::Color defaultColor = sf::Color(0, 255, 0, 255);
const sf::Color collisionColor = sf::Color(255, 0, 0, 255);
const sf::Color mouseRectColor = sf::Color::Magenta;
const sf::Color quadTreeColor = sf::Color::White;

class MainScreen : public State {
   public:
    explicit MainScreen(StateManager* stateManager);

    void processEvents(const sf::Event& event) override;
    void update(const sf::Time& dt) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

   private:
    bool showQuadTree = false;
    bool showMouseRect = false;
    float particleSpeed = 100.f;

    QuadTree<Particle> quadTree;
    sf::FloatRect boundary;
    ushort treeNodeCapacity;

    std::vector<Particle> myObjects;
    std::vector<Particle*> myCollisions;

    sf::RectangleShape mouseRect;

    bool pause, brushMode, pressed;

    sf::Font* font;

    ushort objectNum;

    float radius;

    std::vector<TextBox> textboxes;
    std::vector<sf::Text> labels;
    sf::Text fpsLabel;

    sf::Clock fpsTimer;

    std::vector<Button> buttons;

    void init();

    void initializeObjects();

    void moveObjects(float dt);

    void brush();

    void resize(const sf::Event& event);
};
