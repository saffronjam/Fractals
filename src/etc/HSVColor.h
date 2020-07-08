#pragma once

#include <SFML/Graphics/Color.hpp>

#include "Lib.h"

class HSVColor : protected sf::Color
{
public:
    HSVColor(int hue, float saturation, float value);
    HSVColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 255);
    HSVColor(const sf::Color &color);
    HSVColor(const HSVColor &hsvColor);

    sf::Color GetRGBA();

    int GetHue();
    float GetSaturation();
    float GetValue();

    void SetHue(int hue);
    void SetSaturation(float saturation);
    void SetValue(float value);

private:
    int m_hue;
    float m_saturation, m_value;
};