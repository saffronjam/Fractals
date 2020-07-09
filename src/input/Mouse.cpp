#include "Mouse.h"

std::map<sf::Mouse::Button, bool> Mouse::m_buttonmap;
std::map<sf::Mouse::Button, bool> Mouse::m_prevButtonmap;
std::map<Mouse::CallbackEvent, std::vector<Mouse::ButtonCallback>> Mouse::m_buttonCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::ScrollCallback>> Mouse::m_scrollCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::MoveCallback>> Mouse::m_moveCallbacks;
std::map<Mouse::CallbackEvent, std::vector<Mouse::EnterLeaveCallback>> Mouse::m_enterLeaveCallbacks;
sf::Vector2f Mouse::m_mousePos = sf::Vector2f(0.0f, 0.0f);
bool Mouse::m_inWindow = true;
float Mouse::m_verticalScrollBuffer = 0.0f;
float Mouse::m_horizontalScrollBuffer = 0.0f;


void Mouse::Update() noexcept
{
    for (auto &[key, state] : m_buttonmap)
        m_prevButtonmap[key] = state;

    m_horizontalScrollBuffer = 0.0f;
    m_verticalScrollBuffer = 0.0f;
}

bool Mouse::IsDown(const sf::Mouse::Button &button) noexcept
{
    return m_buttonmap[button];
}

bool Mouse::IsPressed(const sf::Mouse::Button &button) noexcept
{
    return m_buttonmap[button] && !m_prevButtonmap[button];
}

bool Mouse::IsReleased(const sf::Mouse::Button &button) noexcept
{
    return !m_buttonmap[button] && m_prevButtonmap[button];
}

bool Mouse::AnyButtonDown() noexcept
{
    for (auto &[button, state] : m_buttonmap)
        if (state)
            return true;
    return false;
}

void Mouse::HandleEvent(const sf::Event &event) noexcept
{
    switch (event.type)
    {
    case sf::Event::EventType::MouseButtonPressed:
        HandlePress(event.mouseButton);
        break;
    case sf::Event::EventType::MouseButtonReleased:
        HandleRelease(event.mouseButton);
        break;
    case sf::Event::EventType::MouseMoved:
        HandleMove(event.mouseMove);
        break;
    case sf::Event::EventType::MouseEntered:
        HandleEnter();
        break;
    case sf::Event::EventType::MouseLeft:
        HandleLeave();
        break;
    case sf::Event::EventType::MouseWheelScrolled:
        HandleScroll(event.mouseWheelScroll);
        break;
    default:
        break;
    }
}

void Mouse::HandlePress(const sf::Event::MouseButtonEvent &event) noexcept
{
    auto &vector = m_buttonCallbacks[OnMouseButtonPressed];
    for (auto &callback : vector)
        callback(event);
    m_buttonmap[event.button] = true;
}

void Mouse::HandleRelease(const sf::Event::MouseButtonEvent &event) noexcept
{
    auto &vector = m_buttonCallbacks[OnMouseButtonReleased];
    for (auto &callback : vector)
        callback(event);
    m_buttonmap[event.button] = false;
}

void Mouse::HandleMove(const sf::Event::MouseMoveEvent &event) noexcept
{
    auto &vector = m_moveCallbacks[OnMouseMoved];
    for (auto &callback : vector)
        callback(event);
    if (!m_inWindow && AnyButtonDown() || m_inWindow)
    {
        m_mousePos = sf::Vector2f(event.x, event.y);
    }
}

void Mouse::HandleEnter() noexcept
{
    auto &vector = m_enterLeaveCallbacks[OnMouseEntered];
    for (auto &callback : vector)
        callback();
    m_inWindow = true;
}

void Mouse::HandleLeave() noexcept
{
    auto &vector = m_enterLeaveCallbacks[OnMouseLeft];
    for (auto &callback : vector)
        callback();
    m_inWindow = false;
}

void Mouse::HandleScroll(const sf::Event::MouseWheelScrollEvent &event) noexcept
{
    auto &vector = m_scrollCallbacks[OnMouseWheelScrolled];
    for (auto &callback : vector)
        callback(event);
    if (event.wheel == sf::Mouse::Wheel::HorizontalWheel)
    {
        m_horizontalScrollBuffer += event.delta;
    } else if (event.wheel == sf::Mouse::Wheel::VerticalWheel)
    {
        m_verticalScrollBuffer += event.delta;
    }
}

