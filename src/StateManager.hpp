#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "State.hpp"

constexpr const char* APP_TITLE = "Diamant";

class StateManager {
   private:
	State* state;
	State* nextState;
	sf::RenderWindow window;
	bool IsRunning;
	bool isFullscreen;

	void processEvents();
	void update(sf::Time dt);
	void applyChanges();

   public:
	StateManager(unsigned int width, unsigned int height);
	~StateManager();

	void changeState(State* state);
	const sf::RenderWindow& getWindow() const { return window; };
	void setView(const sf::View& view) { window.setView(view); };

	State* getCurrentState();
	bool running();
	void quit();
	void run();
	unsigned int width, height;

	tgui::Gui gui;
};