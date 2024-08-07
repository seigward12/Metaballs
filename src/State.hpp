#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class State {
   protected:
	StateManager* stateManager;

   public:
	State(StateManager* stateManager) : stateManager{stateManager} {};
	~State() = default;
	virtual void processEvent(const sf::Event& event) = 0;
	virtual void update(const sf::Time& dt) = 0;
	virtual void draw(sf::RenderTarget& target) = 0;
};