#pragma once

#include <SFML/Graphics/Color.hpp>

#include "Lib.h"

class HSVColor : public sf::Color
{
public:
    HSVColor();
    HSVColor(int hue, float saturation, float value);
    HSVColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a = 255);
    HSVColor(const sf::Color &color);
    HSVColor(const HSVColor &hsvColor);

    const sf::Color &GetRGBA() const;

    int GetHue() const;
    float GetSaturation() const;
    float GetValue() const;

    void SetHue(int hue);
    void SetSaturation(float saturation);
    void SetValue(float value);

private:
    void GenerateRGB();

private:
    int m_hue;
    float m_saturation, m_value;
};