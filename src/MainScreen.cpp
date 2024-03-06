#include "MainScreen.hpp"

#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>  //TODO : remove

MainScreen::MainScreen(StateManager* stateManager) : State(stateManager) {
    boundary = sf::FloatRect(10, 10, stateManager->width * 0.75,
                             stateManager->height - 20);

    treeNodeCapacity = 4, objectNum = 8, radius = 25;

    quadTree = std::make_unique<QuadTree<Particle>>(boundary, treeNodeCapacity);

    font.loadFromFile("assets/fonts/arial.ttf");

    initializeObjects();

    mouseRect.setSize(
        sf::Vector2f(stateManager->height / 3.f, stateManager->height / 3.f));
    mouseRect.setFillColor(sf::Color::Transparent);
    mouseRect.setOutlineThickness(2);
    mouseRect.setOutlineColor(mouseRectColor);
    const sf::FloatRect temp = mouseRect.getGlobalBounds();
    mouseRect.setOrigin(temp.left + temp.width / 2, temp.top + temp.height / 2);

    fpsLabel.setFont(font);

    for (int i = 0; i < 4; i++) {
        textboxes.emplace_back(font);
        textboxes[i].setAuthorizedCharacters(
            AUTHORIZED_CHARACTERS::NUMBER_ONLY);
        textboxes[i].setBackgroundColor(sf::Color(100, 100, 100, 100));

        labels.emplace_back();
        labels[i].setFont(font);
    }
    labels[0].setString("Object Num:  ");
    labels[1].setString("Radius:  ");
    labels[2].setString("Speed:  ");
    labels[3].setString("Node Capacity:  ");

    for (int i = 0; i < 7; i++)
        buttons.emplace_back(font);

    buttons[0].setOnAction([&]() {
        for (auto& textbox : textboxes)
            if (textbox.empty() || textbox.getString() == "0" ||
                textbox.getString() == "0.")
                return;

        bool noChange = true;

        if (std::stof(textboxes[1].getString()) != radius) {
            radius = std::stof(textboxes[1].getString());
            highestRadius = 0;
            noChange = false;
        }

        if (std::stof(textboxes[2].getString()) != particleSpeed) {
            particleSpeed = std::stof(textboxes[2].getString());
            noChange = false;
        }

        if (std::stoi(textboxes[3].getString()) != treeNodeCapacity) {
            quadTree = std::make_unique<QuadTree<Particle>>(
                boundary, std::stoi(textboxes[3].getString()));
            noChange = false;
        }

        if (std::stoi(textboxes[0].getString()) != objectNum) {
            objectNum = std::stoi(textboxes[0].getString());
            noChange = false;
        }

        if (!noChange)
            initializeObjects();
    });

    buttons[1].setOnAction([&]() {
        pause = !pause;
        buttons[1].setString(pause ? "Resume" : "Pause");
        buttons[1].setBackgroundColor(pause ? sf::Color(100, 100, 100, 150)
                                            : sf::Color::Black);
    });

    buttons[2].setOnAction([&]() {
        showMouseRect = !showMouseRect;
        buttons[2].setString(showMouseRect ? "Hide Mouse Query"
                                           : "Show Mouse Query");
        buttons[2].setBackgroundColor(
            showMouseRect ? sf::Color(100, 100, 100, 150) : sf::Color::Black);
    });

    buttons[3].setOnAction([&]() {
        showQuadTree = !showQuadTree;
        buttons[3].setString(showQuadTree ? "Hide QuadTree" : "Show QuadTree");
        buttons[3].setBackgroundColor(
            showQuadTree ? sf::Color(100, 100, 100, 150) : sf::Color::Black);
    });

    buttons[4].setOnAction([&]() {
        for (auto& myObject : particles)
            myObject->setPosition(
                sf::Vector2f((rand() % static_cast<int>(boundary.width)),
                             ((rand() % static_cast<int>(boundary.height)))));
    });

    buttons[5].setOnAction([&]() {
        brushMode = !brushMode;
        buttons[5].setString(brushMode ? "Cancel Brush Mode" : "Brush Mode");
        buttons[5].setBackgroundColor(brushMode ? sf::Color(100, 100, 100, 150)
                                                : sf::Color::Black);
    });

    buttons[6].setOnAction([&]() {
        collisionEnabled = !collisionEnabled;
        buttons[6].setString(collisionEnabled ? "Disable collisions"
                                              : "Enable collisions");
    });

    init();
}

void MainScreen::init() {
    float marginRight = stateManager->width / 100;

    if (marginRight < 10)
        marginRight = 10;
    else if (marginRight > 20)
        marginRight = 20;

    float characterSize = stateManager->width / 30;
    if (characterSize < 25)
        characterSize = 25;
    else if (characterSize > 50)
        characterSize = 50;

    float textBoxWidth = stateManager->width / 10;
    if (textBoxWidth < 50)
        textBoxWidth = 50;
    else if (textBoxWidth > 100)
        textBoxWidth = 100;

    for (int i = 0; i < 4; i++) {
        textboxes[i].setTextFormat(sf::Color::White, characterSize);
        textboxes[i].setSize(sf::Vector2f(textBoxWidth, characterSize));
        textboxes[i].setPosition(
            sf::Vector2f(stateManager->width - textBoxWidth - marginRight,
                         stateManager->height * 0.075 * i));

        labels[i].setCharacterSize(characterSize);
        labels[i].setFillColor(sf::Color::White);
        labels[i].setPosition(
            sf::Vector2f(textboxes[i].getGlobalBounds().left -
                             labels[i].getGlobalBounds().width,
                         stateManager->height * 0.075 * i));
    }

    textboxes[0].setString(std::to_string(objectNum));
    textboxes[0].setTextLimit(5);

    textboxes[1].setString(std::to_string(radius).substr(0, 3));
    textboxes[1].setTextLimit(3);

    textboxes[2].setString(std::to_string(particleSpeed).substr(0, 3));
    textboxes[2].setTextLimit(3);

    textboxes[3].setString(std::to_string(treeNodeCapacity));
    textboxes[3].setTextLimit(2);

    fpsLabel.setCharacterSize(characterSize);
    fpsLabel.setFillColor(sf::Color::White);
    fpsLabel.setPosition(0.025f * stateManager->width,
                         0.025f * stateManager->height);

    for (int i = 0; i < buttons.size(); i++) {
        buttons[i].setBorder(sf::Color(100, 100, 100, 100), 2);
        buttons[i].setFont(font);
        buttons[i].setCharacterSize(characterSize);
        buttons[i].setTextColor(sf::Color::White);
        buttons[i].setPosition(
            sf::Vector2f(stateManager->width - marginRight -
                             buttons[i].getGlobalBounds().width / 2,
                         stateManager->height * 0.35 +
                             (stateManager->height * 0.1 * i + 1)));
    }

    buttons[0].setString("Apply");
    buttons[1].setString("Pause");
    buttons[2].setString("Show Mouse Query");
    buttons[3].setString("Show QuadTree");
    buttons[4].setString("Randomize");
    buttons[5].setString("Brush Mode");
    buttons[6].setString("Enable collisions");
}

void MainScreen::processEvent(const sf::Event& event) {
    for (auto& textbox : textboxes)
        textbox.processEvent(event);

    for (auto& button : buttons)
        button.processEvent(event);

    switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left)
                pressed = true;
            if (!brushMode)
                selectParticle();
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left) {
                pressed = false;
                selectedParticle = nullptr;
            }
            break;

        case sf::Event::KeyPressed:
            switch (event.key.code) {
                case sf::Keyboard::Space:
                    pause = !pause;
                    break;

                case sf::Keyboard::M:
                    showMouseRect = !showMouseRect;
                    break;

                case sf::Keyboard::Return:
                    buttons[0].getOnAction()();
                    break;

                default:
                    break;
            }
            break;

        case sf::Event::Resized:
            resize(event);
            break;

        case sf::Event::MouseMoved:
            mousePosition = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
            break;

        default:
            break;
    }
}

void MainScreen::update(const sf::Time& dt) {
    if (fpsTimer.getElapsedTime().asSeconds() >= 1) {
        fpsTimer.restart();
        fpsLabel.setString("FPS: " + std::to_string((int)(1 / dt.asSeconds())));
    }

    for (auto& button : buttons)
        button.update();

    for (auto& textboxe : textboxes)
        textboxe.update();

    if (pressed) {
        if (brushMode) {
            brush();
        } else if (selectedParticle != nullptr) {
            sf::Vector2f velocity =
                sf::Vector2f(mousePosition.x - oldMousePosition.x,
                             mousePosition.y - oldMousePosition.y);
            velocity /= dt.asSeconds();
            selectedParticle->setVelocity(velocity);
        }
    }

    if (!pause) {
        moveObjects(dt);
    } else if (selectedParticle != nullptr) {
        selectedParticle->setPosition(mousePosition);
    }

    quadTree->reset();
    for (auto& particle : particles) {
        quadTree->insert(particle.get());
        particle->setColor(defaultColor);
    }

    for (auto& particle : particles) {
        quadTree->query(particle->getGlobalBounds(), myCollisions);

        for (const auto& myCollision : myCollisions) {
            if (particle.get() == myCollision)
                continue;

            if (particle->isColliding(*myCollision)) {
                particle->setColor(collisionColor);
                myCollision->setColor(collisionColor);
            }
        }

        myCollisions.clear();
    }

    if (showMouseRect) {
        mouseRect.setPosition(mousePosition);
        quadTree->query(mouseRect.getGlobalBounds(), myCollisions);

        for (const auto& myCollision : myCollisions)
            myCollision->setColor(mouseRectColor);

        myCollisions.clear();
    }

    if (selectedParticle != nullptr)
        selectedParticle->setColor(sf::Color::Yellow);

    oldMousePosition = mousePosition;
}

void MainScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showQuadTree)
        target.draw(*quadTree);

    for (auto& myObject : particles)
        target.draw(*myObject);

    if (showMouseRect)
        target.draw(mouseRect);

    target.draw(fpsLabel);

    for (int i = 0; i < textboxes.size(); i++) {
        target.draw(labels[i]);
        target.draw(textboxes[i]);
    }

    for (const auto& button : buttons)
        target.draw(button);

    if (selectedParticle != nullptr)
        selectedParticle->setColor(sf::Color::Yellow);
}

void MainScreen::moveObjects(const sf::Time& dt) {
    for (auto& myObject : particles)
        myObject->update(dt, boundary);
}

void MainScreen::initializeObjects() {
    particles.clear();
    for (unsigned short i = 0; i < objectNum; i++) {
        addParticle(sf::Vector2f((rand() % (int)boundary.width),
                                 ((rand() % (int)boundary.height))));
    }
}

void MainScreen::brush() {
    if (mousePosition.x > boundary.left + boundary.width ||
        mousePosition.x < boundary.left ||
        mousePosition.y > boundary.top + boundary.height ||
        mousePosition.y < boundary.top)
        return;

    addParticle(sf::Vector2f(mousePosition.x, mousePosition.y));
    objectNum++;
    textboxes[0].setString(std::to_string(objectNum));
}

void MainScreen::addParticle(const sf::Vector2f& position) {
    particles.push_back(
        std::make_unique<Particle>((radius / 2) + rand() % (int)radius));
    particles.back()->setPosition(position);
    particles.back()->setVelocity(
        sf::Vector2f((rand() % (int)particleSpeed - (particleSpeed) / 2),
                     (rand() % (int)particleSpeed - (particleSpeed) / 2)));
    if (particles.back()->getRadius() > highestRadius)
        highestRadius = particles.back()->getRadius();
}

void MainScreen::selectParticle() {
    sf::FloatRect mouseRect = sf::FloatRect(
        mousePosition.x - highestRadius, mousePosition.y - highestRadius,
        highestRadius * 2, highestRadius * 2);

    quadTree->query(mouseRect, myCollisions);

    if (!myCollisions.empty()) {
        float minDistanceSquare = highestRadius * highestRadius;
        for (Particle* particle : myCollisions) {
            sf::Vector2f distance = particle->getPosition() - mousePosition;
            float distanceSquare =
                distance.x * distance.x + distance.y * distance.y;
            if (distanceSquare <=
                    particle->getRadius() * particle->getRadius() &&
                distanceSquare < minDistanceSquare) {
                selectedParticle = particle;
            }
        }
        if (selectedParticle != nullptr) {
            selectedParticle->setVelocity(sf::Vector2f(0, 0));
            selectedParticle->setPosition(mousePosition);
        }
        myCollisions.clear();
    }
}

void MainScreen::resize(const sf::Event& event) {
    const sf::FloatRect visibleArea =
        sf::FloatRect(0, 0, event.size.width, event.size.height);
    stateManager->setView(sf::View(visibleArea));

    stateManager->width = event.size.width;
    stateManager->height = event.size.height;
    boundary = sf::FloatRect(10, 10, stateManager->width * 0.75,
                             stateManager->height - 20);
    quadTree = std::make_unique<QuadTree<Particle>>(boundary, treeNodeCapacity);

    for (auto& myObject : particles) {
        if (myObject->getPosition().x > boundary.width)
            myObject->setPosition(
                sf::Vector2f(boundary.width, myObject->getPosition().y));

        if (myObject->getPosition().y > boundary.height)
            myObject->setPosition(
                sf::Vector2f(myObject->getPosition().x, boundary.height));
    }

    init();
}
