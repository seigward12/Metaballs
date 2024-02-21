#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "Button.hpp"
#include "Game.hpp"
#include "Particle.hpp"
#include "QuadTree.hpp"
#include "TextBox.hpp"

const sf::Color defaultColor = sf::Color(0, 255, 0, 255);
const sf::Color collisionColor = sf::Color(255, 0, 0, 255);
const sf::Color mouseRectColor = sf::Color::Magenta;
const sf::Color quadTreeColor = sf::Color::White;

class MainScreen : public GameState {
   public:
    explicit MainScreen(Game* game);

    void handleInput() override;
    void update(float dt) override;
    void draw() override;

   private:
    Game* game;
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
