#pragma once

#include <map>

#include "EventMgr.h"

class Keyboard : public EventHandler
{
public:
    using Callback = std::function<void(const sf::Event::KeyEvent &)>;
    enum CallbackEvent
    {
        OnKeyPressed,
        OnKeyReleased
    };

public:
    Keyboard() = default;
    ~Keyboard() = default;
    Keyboard(const Keyboard &) = delete;
    Keyboard &operator=(const Keyboard &) = delete;

    // Move keymap into prev-keymap
    static void Update() noexcept;

    template<Keyboard::CallbackEvent EventType, typename T>
    void AddCallback(const T& callback) noexcept;

    static bool IsDown(const sf::Keyboard::Key &key) noexcept;
    static bool IsPressed(const sf::Keyboard::Key &key) noexcept;
    static bool IsReleased(const sf::Keyboard::Key &key) noexcept;
    static bool IsAnyDown() noexcept;
    static std::string GetTextInput() noexcept { return m_textInputBuffer; }

private:
    void HandleEvent(const sf::Event &event) noexcept override;

    static void HandlePress(const sf::Event::KeyEvent &event) noexcept;
    static void HandleRelease(const sf::Event::KeyEvent &event) noexcept;
    static void HandleTextInput(unsigned char character) noexcept;

private:
    static std::map<sf::Keyboard::Key, bool> m_keymap;
    static std::map<sf::Keyboard::Key, bool> m_prevKeymap;
    static std::map<CallbackEvent, std::vector<Callback>> m_callbacks;
    static std::string m_textInputBuffer;
};

template<Keyboard::CallbackEvent EventType, typename T>
void Keyboard::AddCallback(const T& callback) noexcept
{
    if constexpr(EventType == OnKeyPressed ||
                 EventType == OnKeyReleased)
    {
        m_callbacks[EventType].push_back(callback);
    }
}