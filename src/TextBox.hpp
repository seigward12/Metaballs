#pragma once
#include <SFML/Graphics.hpp>

const sf::Color BORDER_COLOR = sf::Color::White;
const sf::Color BORDER_COLOR_HOVER = sf::Color::Green;
const sf::Color BORDER_COLOR_SELECTED = sf::Color::Red;

class TextBox : public sf::Drawable {
   public:
    explicit TextBox(const sf::Font& font);

    void processEvent(const sf::Event& event);

    void update();

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void setSize(sf::Vector2f size);
    void setPosition(sf::Vector2f position);
    void setOrigin(sf::Vector2f origin);

    void setTextFormat(sf::Color color, float size);
    void setTextColor(sf::Color color);

    void setTextLimit(int limit);

    void setString(const std::string& str);

    void setBackgroundColor(sf::Color color);

    void setSelected(bool selected);

    void allowNumberOnly();

    void allowAlphaOnly();

    void allowAlphaNumeric();

    bool isHovered() const;

    bool isSelected() const;

    bool empty() const;

    sf::Vector2f getPosition() const;

    sf::FloatRect getGlobalBounds() const;
    sf::FloatRect getLocalBounds() const;

    std::string getString() const;

    int getCharacterSize() const;

   private:
    sf::RectangleShape border;
    sf::Text text;

    sf::Vector2f position;
    sf::Vector2f origin;

    int textLimit, subsetCounter, maxCharactersDisplayed;

    bool selected, hover, validTextEntered;

    std::string inputString;

    float typeAllowed{};

    enum {
        BACKSPACE = 8,
        SPACE = 32,
        PERIOD = 46,
        ZERO = 48,
        NINE = 57,
        A = 65,
        Z = 90,
        A_CAPS = 97,
        Z_CAPS = 122
    };

    enum { NUMBER_ONLY, ALPHA_ONLY, ALPHA_NUMERIC };

    float getCharacterWidth() const;
    int calculateMaxCharactersDisplayed();
};