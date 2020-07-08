#include "HSVColor.h"

HSVColor::HSVColor(int hue, float saturation, float value)
    : sf::Color(Lib::HSVtoRGB(hue, saturation, value)),
      m_hue(hue),
      m_saturation(saturation), m_value(value)
{
}

HSVColor::HSVColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a)
    : HSVColor(sf::Color(r, g, b, a))
{
}

HSVColor::HSVColor(const sf::Color &color)
    : HSVColor(Lib::RGBtoHSV(color))
{
}

HSVColor::HSVColor(const HSVColor &hsvColor)
    : HSVColor(hsvColo)
{
}

const sf::Color &HSVColor::GetRGBA()
{
    return *this;
}

int HSVColor::GetHue()
{
    return m_hue;
}

float HSVColor::GetSaturation()
{
    return m_saturation;
}

float HSVColor::GetValue()
{
    return m_value;
}

void HSVColor::SetHue(int hue)
{
    m_hue = hue;
}

void HSVColor::SetSaturation(float saturation)
{
    m_saturation = saturation;
}

void HSVColor::SetValue(float value)
{
    m_value = value;
}
