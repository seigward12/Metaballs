#pragma once
#include <SFML/Graphics.hpp>

class StateManager;

class State : public sf::Drawable {
   protected:
    StateManager* stateManager;

   public:
    State(StateManager* stateManager) : stateManager{stateManager} {};
    virtual void processEvents(const sf::Event& event) = 0;
    void update(const sf::Time& dt) = 0;
};