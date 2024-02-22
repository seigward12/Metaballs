#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Button : public sf::Drawable {
   public:
    Button();
    explicit Button(const sf::Font& font);

    void processEvent(const sf::Event& event);
    void update();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setOrigin(sf::Vector2f origin);
    void setPosition(sf::Vector2f position);
    void setDisabled(bool disabled);
    void setPadding(float padding);

    void setFont(const sf::Font& font);
    void setBorder(sf::Color color, int thickness);
    void setText(const std::string& text, sf::Color color);
    void setString(const std::string& text);
    void setCharacterSize(int size);
    void setTextColor(sf::Color color);
    void setBackgroundColor(sf::Color color);
    void setBackground(sf::Texture& texture);

    void setOnAction(const std::function<void()>& onAction);

    bool isMouseOver() const;

    sf::FloatRect getGlobalBounds() const;
    sf::FloatRect getLocalBounds() const;
    sf::Vector2f getPosition() const;
    sf::Color getTextColor() const;
    std::function<void()> getOnAction() const;

   private:
    void init();

    void resize();

    sf::RectangleShape border;
    sf::Font font;
    sf::Text text;

    bool mouseOver, pressed, disabled;

    sf::Color backgroundColor, borderColor, textColor, hoverColor;

    std::function<void()> onAction;
};