#include <SFML/Graphics.hpp>

#include "MainScreen.hpp"
#include "StateManager.hpp"

int main() {
	if (!sf::Shader::isAvailable()) {
		return -1;
	}
	srand(time(NULL));
	const unsigned int screenWidth = static_cast<unsigned int>(
		sf::VideoMode::getDesktopMode().width * 0.75f);
	const unsigned int screenHeight = static_cast<unsigned int>(
		sf::VideoMode::getDesktopMode().height * 0.75f);
	StateManager stateManager(screenWidth, screenHeight);
	stateManager.changeState(new MainScreen(&stateManager));
	stateManager.run();
}
