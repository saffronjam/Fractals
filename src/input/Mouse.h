#pragma once

#include <map>

#include <SFML/Window/Mouse.hpp>

#include "EventMgr.h"

class Mouse : public EventHandler
{
public:
    using ButtonCallback = std::function<void(const sf::Event::MouseButtonEvent &)>;
    using ScrollCallback = std::function<void(const sf::Event::MouseWheelScrollEvent &)>;
    using MoveCallback = std::function<void(const sf::Event::MouseMoveEvent &)>;
    using EnterLeaveCallback = std::function<void()>;

    enum CallbackEvent
    {
        OnMouseWheelScrolled,
        OnMouseButtonPressed,
        OnMouseButtonReleased,
        OnMouseMoved,
        OnMouseEntered,
        OnMouseLeft
    };

public:
    Mouse() = default;
    ~Mouse() = default;
    Mouse(const Mouse &) = delete;
    Mouse &operator=(const Mouse &) = delete;

    // Move buttonMap into prev-buttonMap
    static void Update() noexcept;

    template<Mouse::CallbackEvent EventType, typename T>
    static void AddCallback(const T &callback) noexcept;

    static bool IsDown(const sf::Mouse::Button &button) noexcept;
    static bool IsPressed(const sf::Mouse::Button &button) noexcept;
    static bool IsReleased(const sf::Mouse::Button &button) noexcept;
    static bool AnyButtonDown() noexcept;

    static sf::Vector2f GetPos() noexcept { return m_mousePos; }
    static float GetVerticalScroll() noexcept { return m_verticalScrollBuffer; }
    static float GetHorizontalScroll() noexcept { return m_horizontalScrollBuffer; }

private:
    void HandleEvent(const sf::Event &event) noexcept override;

    static void HandlePress(const sf::Event::MouseButtonEvent &event) noexcept;
    static void HandleRelease(const sf::Event::MouseButtonEvent &event) noexcept;
    static void HandleMove(const sf::Event::MouseMoveEvent &event) noexcept;
    static void HandleEnter() noexcept;
    static void HandleLeave() noexcept;
    static void HandleScroll(const sf::Event::MouseWheelScrollEvent &event) noexcept;

private:
    static std::map<sf::Mouse::Button, bool> m_buttonmap;
    static std::map<sf::Mouse::Button, bool> m_prevButtonmap;
    static std::map<CallbackEvent, std::vector<ButtonCallback>> m_buttonCallbacks;
    static std::map<CallbackEvent, std::vector<ScrollCallback>> m_scrollCallbacks;
    static std::map<CallbackEvent, std::vector<MoveCallback>> m_moveCallbacks;
    static std::map<CallbackEvent, std::vector<EnterLeaveCallback>> m_enterLeaveCallbacks;

    static sf::Vector2f m_mousePos;
    static bool m_inWindow;

    static float m_verticalScrollBuffer;
    static float m_horizontalScrollBuffer;
};

template<Mouse::CallbackEvent EventType, typename T>
void Mouse::AddCallback(const T &callback) noexcept
{
    if constexpr (EventType == OnMouseWheelScrolled)
    {
        m_scrollCallbacks[EventType].push_back(callback);
    } else if constexpr (EventType == OnMouseButtonPressed || EventType == OnMouseButtonReleased)
    {
        m_buttonCallbacks[EventType].push_back(callback);
    } else if constexpr (EventType == OnMouseMoved)
    {
        m_moveCallbacks[EventType].push_back(callback);
    } else if constexpr (EventType == OnMouseEntered || EventType == OnMouseLeft)
    {
        m_enterLeaveCallbacks[EventType].push_back(callback);
    }
}