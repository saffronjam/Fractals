#include "Lib.h"

#include "HSVColor.h"

sf::Vector2f Lib::Mid(const sf::ConvexShape &polygon) noexcept
{
    size_t nPoints = polygon.getPointCount();
    sf::Vector2f total(0.0f, 0.0f);
    for (size_t i = 0; i < nPoints; i++)
        total += polygon.getPoint(i);
    return total / static_cast<float>(nPoints);
}

sf::Vector2f Lib::Mid(const std::vector<sf::Vector2f> &polygonPoints) noexcept
{
    size_t nPoints = polygonPoints.size();
    if (nPoints > 0)
    {
        sf::Vector2f total(0.0f, 0.0f);
        for (auto &point : polygonPoints)
            total += point;
        return total / static_cast<float>(nPoints);
    }
    else
    {
        return vl::Null<>();
    }
}

void Lib::Rotate(sf::Transformable &transformable, const sf::Vector2f &direction)
{
    sf::Vector2f right(1.0f, 0.0f);

    float angle = Lib::ToDegress(vl::Angle(right, direction));
    if (direction.y < 0.0f)
    {
        angle = 360.0f - angle;
    }
    transformable.setRotation(angle);
}

sf::ConvexShape Lib::CreateConvexShape(const std::vector<sf::Vector2f> &points)
{
    std::vector<sf::Vector2f> usablePoints = Lib::WrapPoints(points);

    sf::ConvexShape finalShape;
    finalShape.setPointCount(usablePoints.size() / 2);

    for (size_t i = 0, currentPoint = 0; i < usablePoints.size() - 1; i += 2, currentPoint++)
    {
        finalShape.setPoint(currentPoint, usablePoints[i]);
    }

    return finalShape;
}

sf::Color Lib::HSVtoRGB(int hue, float saturation, float value)
{
    hue %= 360;
    while (hue < 0)
        hue += 360;

    if (saturation < 0.f)
        saturation = 0.f;
    if (saturation > 1.f)
        saturation = 1.f;

    if (value < 0.f)
        value = 0.f;
    if (value > 1.f)
        value = 1.f;

    int h = hue / 60;
    float f = float(hue) / 60 - h;
    float p = value * (1.f - saturation);
    float q = value * (1.f - saturation * f);
    float t = value * (1.f - saturation * (1 - f));

    switch (h)
    {
    default:
    case 0:
    case 6:
        return sf::Color(value * 255, t * 255, p * 255);
    case 1:
        return sf::Color(q * 255, value * 255, p * 255);
    case 2:
        return sf::Color(p * 255, value * 255, t * 255);
    case 3:
        return sf::Color(p * 255, q * 255, value * 255);
    case 4:
        return sf::Color(t * 255, p * 255, value * 255);
    case 5:
        return sf::Color(value * 255, p * 255, q * 255);
    }
}

sf::Color Lib::HSVtoRGB(const HSVColor &hsvColor)
{
    return HSVtoRGB(hsvColor.GetHue(), hsvColor.GetSaturation(), hsvColor.GetValue());
}

HSVColor Lib::RGBtoHSV(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b)
{
    auto _r = r / 255.0f;
    auto _g = g / 255.0f;
    auto _b = b / 255.0f;

    auto cMax = std::max({_r, _g, _b});
    auto cMin = std::min({_r, _g, _b});

    auto cDelta = cMax - cMin;

    int hue;
    if (cDelta == 0.0f)
    {
        hue = 0;
    }
    else if (cMax == _r)
    {
        hue = 60 * (static_cast<int>((_g - _b) / cDelta) % 6);
    }
    else if (cMax == _g)
    {
        hue = 60 * (static_cast<int>((_b - _r) / cDelta) + 2);
    }
    else if (cMax == _b)
    {
        hue = 60 * (static_cast<int>((_r - _g) / cDelta) + 4);
    }

    float saturation;
    if (cMax == 0.0f)
    {
        saturation = 0.0f;
    }
    else
    {
        saturation = cDelta / cMax;
    }

    auto value = cMax;

    return HSVColor(hue, saturation, value);
}

HSVColor Lib::RGBtoHSV(const sf::Color &color)
{
    return RGBtoHSV(color.r, color.g, color.b);
}