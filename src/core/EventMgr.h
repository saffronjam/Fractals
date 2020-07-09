#pragma once

#include <functional>
#include <vector>

#include <SFML/Window/Event.hpp>

class EventHandler;

class EventMgr
{
public:
    static void PollAll();
    static void AddHandler(EventHandler *handler);

private:
    static std::vector<EventHandler *> m_callbacks;
};

struct EventHandler
{
    EventHandler()
    {
        EventMgr::AddHandler(this);
    }
    virtual void HandleEvent(const sf::Event &) = 0;
};