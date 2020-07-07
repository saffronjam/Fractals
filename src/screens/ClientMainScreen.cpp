#include "ClientMainScreen.h"
#include "AppClient.h"

ClientMainScreen::ClientMainScreen(AppClient &parent)
    : m_parent(parent)
{
}

ClientMainScreen::~ClientMainScreen()
{
}

void ClientMainScreen::Build()
{
}

void ClientMainScreen::Destroy()
{
}

void ClientMainScreen::OnEntry()
{
    LightningMgr::SetAmbientLight(sf::Color(255, 255, 255));
    Camera::SetZoom(200.0f);
}

void ClientMainScreen::OnExit()
{
}

void ClientMainScreen::Update()
{
    m_fractalMgr.Update();
}

void ClientMainScreen::Draw()
{
    m_fractalMgr.Draw();
}

int ClientMainScreen::GetNextScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}

int ClientMainScreen::GetPreviousScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}