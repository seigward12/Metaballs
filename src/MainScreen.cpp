#include "MainScreen.hpp"

#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ToggleButton.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <asssets.hpp>
#include <functional>
#include <stack>

constexpr const char* STRICLY_POSITIVE_INT_REGEX = "^[1-9][0-9]*$";
constexpr uint8_t BUTTON_TEXT_SIZE = 28;
constexpr float BUTTONS_SPACING = 0.05f;

namespace BoundsTransform {
BoundsTransformFct scaleParticle =
	[](const sf::FloatRect& bounds) -> sf::FloatRect {
	sf::FloatRect trasformedBounds(bounds);
	trasformedBounds.top -= bounds.height;
	trasformedBounds.left -= bounds.width;
	trasformedBounds.width *= 3.0f;
	trasformedBounds.height *= 3.0f;
	return trasformedBounds;
};
}  // namespace BoundsTransform

sf::Vector2f getRandomVelocity(const float velocity) {
	return velocity == 0.f
			   ? sf::Vector2f()
			   : sf::Vector2f((rand() % (int)velocity - (velocity) / 2),
							  (rand() % (int)velocity - (velocity) / 2));
}

MainScreen::MainScreen(StateManager* stateManager)
	: State{stateManager}, radius{25}, treeNodeCapacity{4}, particleSpeed{100} {
	boundary = sf::FloatRect(10, 10, stateManager->width * 0.75f,
							 stateManager->height - 20);
	boundaryShape = sf::RectangleShape();
	boundaryShape.setPosition(boundary.getPosition());
	boundaryShape.setSize(boundary.getSize());
	boundaryShape.setFillColor(sf::Color::Black);

	quadTree = std::make_unique<QuadTree<Particle>>(boundary, treeNodeCapacity);
	setNewMaxRadius(radius);

	bool fontResult = font.loadFromFile(ARIAL_FONT);
	bool shaderResult = metaballsShader.loadFromFile(
		METABALLS_FRAG_SHADER, sf::Shader::Type::Fragment);
#ifdef _DEBUG
	if (!fontResult) {
		std::cerr << "Error: loading font failed" << std::endl;
		exit(1);
	}
	if (!shaderResult) {
		std::cerr << "Error: loading metaballs shader" << std::endl;
		exit(1);
	}
#endif
	fpsLabel.setFont(font);
	metaballsShader.setUniform(
		"u_resolution",
		sf::Glsl::Vec2(stateManager->width, stateManager->height));

	initializeObjects(10);

	mouseRect.setSize(
		sf::Vector2f(stateManager->height / 3.f, stateManager->height / 3.f));
	mouseRect.setFillColor(sf::Color::Transparent);
	mouseRect.setOutlineThickness(2);
	mouseRect.setOutlineColor(MOUSE_RECT_COLOR);
	const sf::FloatRect temp = mouseRect.getGlobalBounds();
	mouseRect.setOrigin(temp.left + temp.width / 2, temp.top + temp.height / 2);

	// set up tgui buttons
	tgui::Font tguiFont(ARIAL_FONT);  // TODO charger a partir du
									  // sfml font ou le contraire
	tgui::Font::setGlobalFont(tguiFont);
	tgui::VerticalLayout::Ptr verticalSideBar = tgui::VerticalLayout::create();
	verticalSideBar->setPosition(boundary.getPosition().x + boundary.width, 0);
	verticalSideBar->setSize(stateManager->width - boundary.width,
							 stateManager->height);
	stateManager->gui.add(verticalSideBar);

	tgui::HorizontalLayout::Ptr horizontalLayout =
		tgui::HorizontalLayout::create();
	particulesCountInput = tgui::EditBox::create();
	particulesCountInput->setInputValidator(tgui::EditBox::Validator::UInt);
	particulesCountInput->setTextSize(BUTTON_TEXT_SIZE);
	particulesCountInput->setText(std::to_string(particles.size()));
	particulesCountInput->onReturnKeyPress(&MainScreen::setParticuleCount,
										   this);
	tgui::Label::Ptr label = tgui::Label::create("Particules number");
	label->setTextSize(BUTTON_TEXT_SIZE);
	label->getRenderer()->setTextColor(tgui::Color::White);
	label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
	horizontalLayout->add(label);				  // index 0
	horizontalLayout->add(particulesCountInput);  // index 1
	verticalSideBar->add(horizontalLayout);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	horizontalLayout = tgui::HorizontalLayout::copy(horizontalLayout);
	tgui::EditBox::Ptr radiusInput =
		horizontalLayout->get(1)->cast<tgui::EditBox>();
	radiusInput->setInputValidator(tgui::EditBox::Validator::Float);
	radiusInput->setText(std::to_string(radius));
	radiusInput->onReturnKeyPress(&MainScreen::setParticuleRadius, this);
	horizontalLayout->get(0)->cast<tgui::Label>()->setText("Radius");
	verticalSideBar->add(horizontalLayout);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	horizontalLayout = tgui::HorizontalLayout::copy(horizontalLayout);
	tgui::EditBox::Ptr speedInput =
		horizontalLayout->get(1)->cast<tgui::EditBox>();
	speedInput->setInputValidator(tgui::EditBox::Validator::Float);
	speedInput->setText(std::to_string(particleSpeed));
	speedInput->onReturnKeyPress(&MainScreen::setParticuleSpeed, this);
	horizontalLayout->get(0)->cast<tgui::Label>()->setText("Speed");
	verticalSideBar->add(horizontalLayout);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	horizontalLayout = tgui::HorizontalLayout::copy(horizontalLayout);
	tgui::EditBox::Ptr nodeCapacityInput =
		horizontalLayout->get(1)->cast<tgui::EditBox>();
	nodeCapacityInput->setInputValidator(STRICLY_POSITIVE_INT_REGEX);
	nodeCapacityInput->setText(std::to_string(treeNodeCapacity));
	nodeCapacityInput->onReturnKeyPress(&MainScreen::setTreeNodeCapacity, this);
	horizontalLayout->get(0)->cast<tgui::Label>()->setText("Node capacity");
	verticalSideBar->add(horizontalLayout);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	tgui::ToggleButton::Ptr toggle = tgui::ToggleButton::create();
	toggle->onToggle([toggle, this](bool isPaused) {
		toggle->setText(isPaused ? "Resume" : "Pause");
		this->setPaused(isPaused);
	});
	toggle->onToggle.emit(toggle.get(), isPaused);
	toggle->setTextSize(BUTTON_TEXT_SIZE);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	toggle = tgui::ToggleButton::copy(toggle);
	toggle->onToggle([toggle, this](bool isShowingQuery) {
		toggle->setText(isShowingQuery ? "Hide mouse query"
									   : "Show mouse query");
		this->setMouseRectVisibility(isShowingQuery);
	});
	toggle->onToggle.emit(toggle.get(), showMouseRect);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	toggle = tgui::ToggleButton::copy(toggle);
	toggle->onToggle([toggle, this](bool isShowingQuadTree) {
		toggle->setText(isShowingQuadTree ? "Hide QuadTree" : "Show QuadTree");
		this->setQuadTreeVisibility(isShowingQuadTree);
	});
	toggle->onToggle.emit(toggle.get(), showQuadTree);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	toggle = tgui::ToggleButton::copy(toggle);
	toggle->onToggle([toggle, this](bool isBrushModeEnabled) {
		toggle->setText(isBrushModeEnabled ? "Disable Brush Mode"
										   : "Enable Brush Mode");
		this->enableBrushMode(isBrushModeEnabled);
	});
	toggle->onToggle.emit(toggle.get(), brushMode);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	toggle = tgui::ToggleButton::copy(toggle);
	toggle->onToggle([toggle, this](bool isCollisionEnabled) {
		toggle->setText(isCollisionEnabled ? "Disable Collisions"
										   : "Enable Collisions");
		this->enableCollisions(isCollisionEnabled);
	});
	toggle->onToggle.emit(toggle.get(), collisionEnabled);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	toggle = tgui::ToggleButton::copy(toggle);
	toggle->onToggle([toggle, this](bool isShaderEnabled) {
		toggle->setText(isShaderEnabled ? "Disable metaballs effect"
										: "Enable metaballs effect");
		this->enableShader(isShaderEnabled);
	});
	toggle->onToggle.emit(toggle.get(), collisionEnabled);
	verticalSideBar->add(toggle);
	verticalSideBar->addSpace(BUTTONS_SPACING);

	tgui::Button::Ptr button = tgui::Button::create("Apply");
	button->onClick(&MainScreen::initializeObjects, this, -1);
	button->setTextSize(BUTTON_TEXT_SIZE);
	verticalSideBar->add(button);
}

void MainScreen::processEvent(const sf::Event& event) {
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
				if (selectedParticle != nullptr) {
					selectedParticle->setInfiniteMass(false);
					selectedParticle = nullptr;
				}
			}
			break;

		case sf::Event::KeyPressed:
			switch (event.key.code) {
				case sf::Keyboard::Space:
					isPaused = !isPaused;
					break;

				case sf::Keyboard::M:
					showMouseRect = !showMouseRect;
					break;

				case sf::Keyboard::Return:
					// buttons[0].getOnAction()();
					break;

				default:
					break;
			}
			break;

		case sf::Event::Resized:
			resize(sf::Vector2f(event.size.width, event.size.height));
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

	if (!isPaused) {
		moveObjects(dt);
	} else if (selectedParticle != nullptr) {
		selectedParticle->setPosition(mousePosition);
	}

	quadTree->reset();
	for (auto& particle : particles) {
		quadTree->insert(particle.get());
		particle->setColor(DEFAULT_COLOR);
	}

	for (auto& particle : particles) {
		quadTree->query(particle->getGlobalBounds(), myCollisions);

		for (Particle* myCollision : myCollisions) {
			if (particle.get() == myCollision)
				continue;

			if (collisionEnabled && !isPaused) {
				particle->collideWithParticle(*myCollision);
			} else {
				if (particle->isColliding(*myCollision)) {
					particle->setColor(COLLISION_COLOR);
					myCollision->setColor(COLLISION_COLOR);
				}
			}
		}

		myCollisions.clear();
	}

	if (showMouseRect) {
		mouseRect.setPosition(mousePosition);
		quadTree->query(mouseRect.getGlobalBounds(), myCollisions);

		for (const auto& myCollision : myCollisions)
			myCollision->setColor(MOUSE_RECT_COLOR);

		myCollisions.clear();
	}

	if (selectedParticle != nullptr)
		selectedParticle->setColor(sf::Color::Yellow);

	oldMousePosition = mousePosition;

	if (shaderEnabled) {
		Particle::setGlobalBoundsTransform(BoundsTransform::scaleParticle);
		particlesGroupsForShader.clear();
		std::unordered_map<Particle*, std::unordered_set<Particle*>*>
			visitedParticles;
		std::unordered_set<Particle*> neighbors;
		std::function<void(const Particle* const)> scaleAndQueryParticle =
			[&](const Particle* const particleToScaleAndQuery) {
				neighbors.clear();
				quadTree->query(particleToScaleAndQuery->getGlobalBounds(),
								neighbors);
			};
		for (auto& particlePtr : particles) {
			Particle* particle = particlePtr.get();
			if (visitedParticles.contains(particle))
				continue;

			scaleAndQueryParticle(particle);

			Particle* firstAlreadyVisitedParticle = nullptr;
			std::unordered_set<Particle*>* particleGroup = nullptr;
			for (Particle* neighbor : neighbors) {
				if (visitedParticles.contains(neighbor)) {
					firstAlreadyVisitedParticle = neighbor;
					break;
				}
			}
			if (firstAlreadyVisitedParticle == nullptr) {
				particlesGroupsForShader.emplace(
					particle, std::unordered_set<Particle*>());
				particleGroup = &particlesGroupsForShader.at(particle);
			} else
				particleGroup =
					visitedParticles.at(firstAlreadyVisitedParticle);

			for (Particle* neighbor : neighbors) {
				if (visitedParticles.contains(neighbor)) {
					std::unordered_set<Particle*>* neighborGroup =
						visitedParticles.at(neighbor);
					if (neighborGroup == particleGroup)
						continue;
					Particle* groupToRemoveKey = nullptr;
					for (Particle* neighborOfNeighbor : *neighborGroup) {
						if (particlesGroupsForShader.contains(
								neighborOfNeighbor))
							groupToRemoveKey = neighborOfNeighbor;
						visitedParticles[neighborOfNeighbor] = neighborGroup;
					}
					particlesGroupsForShader.erase(groupToRemoveKey);
				} else {
					visitedParticles.emplace(neighbor, particleGroup);
					particleGroup->insert(neighbor);
				}
			}
		}
		Particle::resetGlobalBoundsTransfom();
	}
}

void MainScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (shaderEnabled) {
		for (const auto& [_, particleGroup] : particlesGroupsForShader) {
			float top =
				(*std::min_element(particleGroup.begin(), particleGroup.end(),
								   [](Particle* lhs, Particle* rhs) {
									   return lhs->getGlobalBounds().top <
											  rhs->getGlobalBounds().top;
								   }))
					->getGlobalBounds()
					.top;
			float left =
				(*std::min_element(particleGroup.begin(), particleGroup.end(),
								   [](Particle* lhs, Particle* rhs) {
									   return lhs->getGlobalBounds().left <
											  rhs->getGlobalBounds().left;
								   }))
					->getGlobalBounds()
					.left;
			Particle* rightParticle = (*std::max_element(
				particleGroup.begin(), particleGroup.end(),
				[](Particle* lhs, Particle* rhs) {
					return lhs->getCenterPosition().x + lhs->getRadius() <
						   rhs->getCenterPosition().x + rhs->getRadius();
				}));
			float right = rightParticle->getCenterPosition().x +
						  rightParticle->getRadius();
			Particle* downParticle = (*std::max_element(
				particleGroup.begin(), particleGroup.end(),
				[](Particle* lhs, Particle* rhs) {
					return lhs->getCenterPosition().y + lhs->getRadius() <
						   rhs->getCenterPosition().y + rhs->getRadius();
				}));
			float down =
				downParticle->getCenterPosition().y + downParticle->getRadius();
			sf::RectangleShape groupMaxRectangle(
				sf::Vector2f(right - left, down - top));
			groupMaxRectangle.setPosition(sf::Vector2f(left, top));
			sf::FloatRect groupBounds = groupMaxRectangle.getGlobalBounds();
			sf::FloatRect firstBound =
				(*particleGroup.begin())->getGlobalBounds();
			groupMaxRectangle.setFillColor(sf::Color(120, 120, 120, 120));
			target.draw(groupMaxRectangle, states);
			///
			// states.shader = &metaballsShader;
			// target.draw(boundaryShape, states);
			// states.shader = nullptr;
		}
		for (auto& myObject : particles) {
			sf::FloatRect a =
				BoundsTransform::scaleParticle(myObject->getGlobalBounds());
			sf::RectangleShape b(sf::Vector2f(a.width, a.height));
			b.setFillColor(sf::Color(255, 0, 0, 50));
			b.setPosition(a.getPosition());
			target.draw(b, states);
		}
	} else {
		for (auto& myObject : particles)
			target.draw(*myObject, states);
	}

	if (showQuadTree)
		target.draw(*quadTree, states);

	if (showMouseRect)
		target.draw(mouseRect, states);

	target.draw(fpsLabel, states);

	if (selectedParticle != nullptr)
		selectedParticle->setColor(sf::Color::Yellow);
}

void MainScreen::moveObjects(const sf::Time& dt) {
	for (auto& myObject : particles)
		myObject->update(dt, boundary);
}

void MainScreen::initializeObjects(int objectNumber) {
	if (objectNumber < 0)
		objectNumber = particles.size();
	particles.clear();
	for (unsigned short i = 0; i < objectNumber; i++) {
		addParticle(sf::Vector2f((rand() % (int)boundary.width),
								 ((rand() % (int)boundary.height))));
	}
	metaballsShader.setUniform("n_balls", objectNumber);
}

void MainScreen::brush() {
	if (mousePosition.x > boundary.left + boundary.width ||
		mousePosition.x < boundary.left ||
		mousePosition.y > boundary.top + boundary.height ||
		mousePosition.y < boundary.top)
		return;

	addParticle(sf::Vector2f(mousePosition.x, mousePosition.y));
	particulesCountInput->setText(std::to_string(particles.size()));
}

void MainScreen::addParticle(const sf::Vector2f& position) {
	particles.push_back(std::make_unique<Particle>(
		(radius / 2) + rand() % static_cast<int>(radius)));
	particles.back()->setPosition(position);
	particles.back()->setVelocity(getRandomVelocity(particleSpeed));
}

void MainScreen::selectParticle() {
	sf::FloatRect mouseRect = sf::FloatRect(
		mousePosition.x - highestRadius, mousePosition.y - highestRadius,
		highestRadius * 2, highestRadius * 2);

	quadTree->query(mouseRect, myCollisions);

	if (!myCollisions.empty()) {
		float minDistanceSquare = highestRadius * highestRadius;
		for (Particle* particle : myCollisions) {
			sf::Vector2f distance =
				particle->getCenterPosition() - mousePosition;
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
			selectedParticle->setInfiniteMass(true);
		}
		myCollisions.clear();
	}
}

void MainScreen::resize(const sf::Vector2f& dimensions) {
	const sf::FloatRect visibleArea =
		sf::FloatRect(0, 0, dimensions.x, dimensions.y);
	stateManager->setView(sf::View(visibleArea));

	stateManager->width = dimensions.x;
	stateManager->height = dimensions.y;
	boundary = sf::FloatRect(10, 10, stateManager->width * 0.75,
							 stateManager->height - 20);
	quadTree = std::make_unique<QuadTree<Particle>>(boundary, treeNodeCapacity);

	for (auto& myObject : particles) {
		if (myObject->getCenterPosition().x > boundary.width)
			myObject->setPosition(
				sf::Vector2f(boundary.width, myObject->getCenterPosition().y));

		if (myObject->getCenterPosition().y > boundary.height)
			myObject->setPosition(
				sf::Vector2f(myObject->getCenterPosition().x, boundary.height));
	}
}

void MainScreen::setParticuleCount(const tgui::String& particuleCountString) {
	int particuleCount = particuleCountString.toInt();
	if (particles.size() != particuleCount)
		initializeObjects(particuleCount);
}

void MainScreen::setParticuleRadius(const tgui::String& newRadiusString) {
	float newRadius = newRadiusString.toFloat();
	if (radius != newRadius) {
		radius = newRadius;
		setNewMaxRadius(newRadius);
		for (auto& particle : particles) {
			particle->setRadius((radius / 2) +
								rand() % static_cast<int>(radius));
		}
	}
}

void MainScreen::setNewMaxRadius(const float newRadius) {
	highestRadius = newRadius * 1.5f;
}

void MainScreen::setParticuleSpeed(const tgui::String& particuleSpeedString) {
	float newParticuleSpeed = particuleSpeedString.toFloat();
	if (particleSpeed != newParticuleSpeed) {
		particleSpeed = newParticuleSpeed;
		for (auto& particle : particles) {
			particle->setVelocity(getRandomVelocity(particleSpeed));
		}
	}
}

void MainScreen::setTreeNodeCapacity(
	const tgui::String& newTreeNodeCapacityString) {
	int newTreeNodeCapacity = newTreeNodeCapacityString.toUInt();
	if (treeNodeCapacity != newTreeNodeCapacity) {
		treeNodeCapacity = newTreeNodeCapacity;
		quadTree =
			std::make_unique<QuadTree<Particle>>(boundary, treeNodeCapacity);
	}
}
