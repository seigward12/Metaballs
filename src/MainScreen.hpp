#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <vector>

#include <memory>
#include "Particle.hpp"
#include "QuadTree.hpp"
#include "State.hpp"
#include "StateManager.hpp"

#include <TGUI/Widgets/EditBox.hpp>

// // const sf::Color DEFAULT_COLOR = sf::Color::Green;
// // const sf::Color COLLISION_COLOR = sf::Color::Red;
// // const sf::Color MOUSE_RECT_COLOR = sf::Color::Magenta;
// // const sf::Color QUAD_TREE_COLOR = sf::Color::White;
const sf::Color DEFAULT_COLOR = sf::Color(0, 255, 0, 255);
const sf::Color COLLISION_COLOR = sf::Color(255, 0, 0, 255);
const sf::Color MOUSE_RECT_COLOR = sf::Color(255, 0, 255, 255);
const sf::Color QUAD_TREE_COLOR = sf::Color(255, 255, 255, 255);

const size_t SHADER_PARTICLE_GROUP_SIZE = 8;

class MainScreen : public State {
   public:
	explicit MainScreen(StateManager* stateManager);

	void processEvent(const sf::Event& event) override;
	void update(const sf::Time& dt) override;

	// editBoxes methods
	void setParticuleCount(const tgui::String&);
	void setParticuleRadius(const tgui::String&);
	void setParticuleSpeed(const tgui::String&);
	void setTreeNodeCapacity(const tgui::String&);

	// checkboxes methods
	void setPaused(bool _isPaused) { isPaused = _isPaused; };
	void setMouseRectVisibility(bool isVisible) { showMouseRect = isVisible; };
	void setQuadTreeVisibility(bool isVisible) { showQuadTree = isVisible; };
	void enableBrushMode(bool enabled) { brushMode = enabled; };
	void enableCollisions(bool enabled) { collisionEnabled = enabled; };
	void enableShader(bool enabled) { shaderEnabled = enabled; };

   private:
	bool isPaused = false;
	bool brushMode = false;
	bool pressed = false;
	bool showQuadTree = false;
	bool showMouseRect = false;
	bool collisionEnabled = false;
	bool shaderEnabled = false;

	unsigned int treeNodeCapacity;
	float radius;
	float highestRadius;
	float particleSpeed;

	Particle* selectedParticle = nullptr;

	sf::RectangleShape mouseRect;
	sf::Vector2f mousePosition = sf::Vector2f(0, 0);
	sf::Vector2f oldMousePosition = sf::Vector2f(0, 0);

	std::unique_ptr<QuadTree> quadTree;
	sf::FloatRect boundary;
	sf::RectangleShape boundaryShape;

	std::vector<std::unique_ptr<Particle>> particles;

	sf::Font font;
	sf::Text fpsLabel;
	sf::Clock fpsTimer;

	sf::Shader metaballsShader;
	// std::vector<std::array<Particle*, SHADER_PARTICLE_GROUP_SIZE>>
	std::unordered_map<Particle*, std::unordered_set<Particle*>>
		particlesGroupsForShader;

	tgui::EditBox::Ptr particulesCountInput;

	void initializeObjects(int objectNumber);
	void moveObjects(const sf::Time& dt);
	void addParticle(const sf::Vector2f& position);
	void brush();
	void updateSelectedParticle(const sf::Vector2f& position);
	void selectParticle();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void setNewMaxRadius(const float newRadius);
};
