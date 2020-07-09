#include "GuiMgr.h"

sfg::SFGUI GuiMgr::m_sfgui;
sfg::Desktop GuiMgr::m_desktop;

void GuiMgr::HandleEvent(const sf::Event &event) noexcept
{
    m_desktop.HandleEvent(event);
}