#include "Button.hpp"

Button::Button() {
    init();
}

Button::Button(const sf::Font& font) {
    init();
}

void Button::init() {
    mouseOver = pressed = disabled = false;

    border.setFillColor(sf::Color::Transparent);

    text.setString("Button");
    onAction = []() {};
}

void Button::setDisabled(bool disabled) {
    this->disabled = disabled;
}

void Button::setFont(const sf::Font& font) {
    text.setFont(font);
}

void Button::setCharacterSize(const int size) {
    text.setCharacterSize(size);
    border.setSize(sf::Vector2f(text.getGlobalBounds().width * 1.1,
                                text.getGlobalBounds().height * 1.2));
    sf::FloatRect temp = text.getGlobalBounds();
    text.setOrigin(temp.left + temp.width / 2, temp.top + temp.height / 2);
}

void Button::setPosition(const sf::Vector2f position) {
    text.setPosition(position);

    sf::FloatRect temp = text.getLocalBounds();
    text.setOrigin(temp.left + temp.width / 2.0f,
                   temp.top + temp.height / 2.0f);

    temp = border.getLocalBounds();
    border.setOrigin(temp.left + temp.width / 2.0f,
                     temp.top + temp.height / 2.0f);
    border.setPosition(position);
}

void Button::setText(const std::string& text, const sf::Color color) {
    this->text.setString(text);
    this->text.setFillColor(color);
    textColor = color;
    hoverColor = color;

    border.setSize(sf::Vector2f(this->text.getGlobalBounds().width * 1.1,
                                this->text.getGlobalBounds().height * 1.35));
    setPosition(border.getPosition());
}

void Button::setString(const std::string& text) {
    this->text.setString(text);
    border.setSize(sf::Vector2f(this->text.getGlobalBounds().width * 1.1,
                                this->text.getGlobalBounds().height * 1.35));
    setPosition(border.getPosition());
}

void Button::setBorder(const sf::Color color, const int thickness) {
    border.setOutlineThickness(thickness);
    borderColor = color;
    border.setOutlineColor(color);
}

void Button::setBackgroundColor(const sf::Color color) {
    backgroundColor = color;
    border.setFillColor(color);
}

void Button::setOnAction(const std::function<void()>& onAction) {
    this->onAction = onAction;
}

std::function<void()> Button::getOnAction() const {
    return onAction;
}

void Button::processEvent(const sf::Event& event) {
    if (disabled)
        return;

    switch (event.type) {
        case sf::Event::MouseMoved:
            mouseOver = border.getGlobalBounds().contains(event.mouseMove.x,
                                                          event.mouseMove.y);

        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Left && mouseOver) {
                pressed = true;
            }
            break;

        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Left && mouseOver) {
                onAction();
            }
            break;

        default:
            break;
    }
}

void Button::update() {
    if (disabled)
        return;

    if (mouseOver) {
        border.setOutlineColor(hoverColor);
        border.setFillColor(sf::Color::Transparent);
    } else {
        border.setOutlineColor(borderColor);
        border.setFillColor(backgroundColor);
    }

    if (pressed) {  // TODO fix to not set color each update
        text.setFillColor(backgroundColor);
        border.setFillColor(textColor);
    } else {
        text.setFillColor(textColor);
        border.setFillColor(backgroundColor);
    }
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(border);
    target.draw(text);

    if (disabled) {
        sf::RectangleShape disabledShade(border);
        disabledShade.setFillColor(sf::Color(0, 0, 0, 150));
        target.draw(disabledShade);
    }
}

sf::FloatRect Button::getGlobalBounds() const {
    return border.getGlobalBounds();
}

sf::FloatRect Button::getLocalBounds() const {
    return border.getLocalBounds();
}

sf::Vector2f Button::getPosition() const {
    return border.getPosition();
}

void Button::setTextColor(sf::Color color) {
    text.setFillColor(color);
    textColor = color;
    hoverColor = color;
}

sf::Color Button::getTextColor() const {
    return textColor;
}

bool Button::isMouseOver() const {
    return mouseOver;
}