#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include <memory>
#include "Particle.hpp"
#include "QuadTree.hpp"
#include "State.hpp"
#include "StateManager.hpp"

const sf::Color DEFAULT_COLOR = sf::Color(0, 255, 0, 255);
const sf::Color COLLISION_COLOR = sf::Color(255, 0, 0, 255);
const sf::Color MOUSE_RECT_COLOR = sf::Color::Magenta;
const sf::Color QUAD_TREE_COLOR = sf::Color::White;

class MainScreen : public State {
   public:
	explicit MainScreen(StateManager* stateManager);

	void processEvent(const sf::Event& event) override;
	void update(const sf::Time& dt) override;
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void setPaused(bool isPaused) { this->isPaused = isPaused; };
	void setMouseRectVisibility(bool isVisible) { showMouseRect = isVisible; };
	void setQuadTreeVisibility(bool isVisible) { showQuadTree = isVisible; };
	void enableBrushMode(bool enabled) { brushMode = enabled; };
	void enableCollisions(bool enabled) { collisionEnabled = enabled; };

   private:
	bool isPaused = false;
	bool brushMode = false;
	bool pressed = false;
	bool showQuadTree = false;
	bool showMouseRect = false;
	bool collisionEnabled = false;

	float particleSpeed = 100.f;
	Particle* selectedParticle = nullptr;
	sf::RectangleShape mouseRect;
	sf::Vector2f mousePosition = sf::Vector2f(0, 0);
	sf::Vector2f oldMousePosition = sf::Vector2f(0, 0);

	std::unique_ptr<QuadTree<Particle>> quadTree;
	sf::FloatRect boundary;
	unsigned short treeNodeCapacity = 4;
	unsigned short objectNum = 10;
	float radius = 3;
	float highestRadius = 0;

	std::vector<std::unique_ptr<Particle>> particles;
	std::vector<Particle*> myCollisions;

	sf::Font font;
	sf::Text fpsLabel;
	sf::Clock fpsTimer;

	void initializeObjects();
	void moveObjects(const sf::Time& dt);
	void addParticle(const sf::Vector2f& position);
	void brush();
	void resize(const sf::Vector2f& dimensions);
	void updateSelectedParticle(const sf::Vector2f& position);
	void selectParticle();
};
