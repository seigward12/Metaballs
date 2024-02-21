#include <SFML/Graphics.hpp>
#include <iostream>

#include "Game.hpp"
#include "MainScreen.hpp"

typedef unsigned short ushort;

int main() {
    srand(time(NULL));
    const unsigned int screenWidth =
        sf::VideoMode::getDesktopMode().width * 0.75f;
    const unsigned int screenHeight =
        sf::VideoMode::getDesktopMode().height * 0.75f;
    Game myGame(screenWidth, screenHeight);
    myGame.changeScreen(new MainScreen(&myGame));
    myGame.gameLoop();
}