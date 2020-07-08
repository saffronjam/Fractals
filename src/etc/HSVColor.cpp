#include "HSVColor.h"

HSVColor::HSVColor()
    : HSVColor(0, 0.0f, 0.0f)
{
}

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
    : HSVColor(hsvColor.m_hue, hsvColor.m_saturation, hsvColor.m_value)
{
}

const sf::Color &HSVColor::GetRGBA() const
{
    return *this;
}

int HSVColor::GetHue() const
{
    return m_hue;
}

float HSVColor::GetSaturation() const
{
    return m_saturation;
}

float HSVColor::GetValue() const
{
    return m_value;
}

void HSVColor::SetHue(int hue)
{
    m_hue = hue;
    GenerateRGB();
}

void HSVColor::SetSaturation(float saturation)
{
    m_saturation = saturation;
    GenerateRGB();
}

void HSVColor::SetValue(float value)
{
    m_value = value;
    GenerateRGB();
}

void HSVColor::GenerateRGB()
{
    auto color = Lib::HSVtoRGB(m_hue, m_saturation, m_value);
    r = color.r;
    g = color.g;
    b = color.b;
}
