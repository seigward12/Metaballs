#pragma once
#include <sfml/Graphics.hpp>
#include "State.hpp"

const std::string APP_TITLE = "Diamant";
const unsigned int FRAMERATE = 60;
const sf::Time TIME_PER_FRAME = sf::seconds(1.f / FRAMERATE);

class StateManager {
   private:
    State* state;
    State* nextState;
    bool m_running;
    sf::RenderWindow window;
    bool isFullscreen;

    void processEvents();
    void update(sf::Time dt);
    void applyChanges();

   public:
    StateManager(float width, float height);
    ~StateManager();

    void changeState(State* state);

    State* getCurrentState();
    bool running();
    void quit();
    void run();
    float width, height;
};