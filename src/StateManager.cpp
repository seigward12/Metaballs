#include <iostream>

#include "StateManager.hpp"

StateManager::StateManager(float width, float height)
    : IsRunning{true},
      window(sf::VideoMode(width, height), "Metaballs", sf::Style::Default),
      state{nullptr},
      width{width},
      height{height} {
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(144);  // TODO
    gui.setWindow(window);
}

StateManager::~StateManager() {
    delete state;
    delete nextState;
}

void StateManager::changeState(State* state) {
    nextState = state;
}

bool StateManager::running() {
    return IsRunning;
}

void StateManager::quit() {
    IsRunning = false;
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
        processEvents();
        update(dt);

        if (state == nullptr)
            window.close();

        window.clear();
        window.draw(*state);
        gui.draw();
        window.display();
    }
}

void StateManager::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        gui.handleEvent(event);
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