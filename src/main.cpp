#include <SFML/Graphics.hpp>
#include <iostream>

#include "MainScreen.hpp"
#include "StateManager.hpp"

int main() {
	srand(time(NULL));
	const unsigned int screenWidth =
		sf::VideoMode::getDesktopMode().width * 0.75f;
	const unsigned int screenHeight =
		sf::VideoMode::getDesktopMode().height * 0.75f;
	StateManager stateManager(screenWidth, screenHeight);
	stateManager.changeState(new MainScreen(&stateManager));
	stateManager.run();
}
