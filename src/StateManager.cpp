#include <iostream>

#include "StateManager.hpp"

StateManager::StateManager(float width, float height)
    : m_running{true},
      window(sf::VideoMode(width, height), "Diamant", sf::Style::Default),
      state{nullptr},
      width{width},
      height{height} {
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorGrabbed(true);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(FRAMERATE);
}

StateManager::~StateManager() {
    delete state;
    delete nextState;
}

void StateManager::changeState(State* state) {
    nextState = state;
}

bool StateManager::running() {
    return m_running;
}

void StateManager::quit() {
    m_running = false;
}

State* StateManager::getCurrentState() {
    return state;
}

void StateManager::run() {
    applyChanges();
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate > TIME_PER_FRAME) {
            timeSinceLastUpdate -= TIME_PER_FRAME;
            processEvents();
            update(timeSinceLastUpdate);

            if (state == nullptr)
                window.close();
        }

        window.clear();
        window.draw(*state);
        window.display();
    }
}

void StateManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        state->processEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
        applyChanges();
    }
}

void StateManager::update(sf::Time dt) {
    state->update(dt);
    applyChanges();
}

void StateManager::applyChanges() {
    if (nextState != nullptr) {
        delete state;
        state = nextState;
        nextState = nullptr;
    }
}