#pragma once

#include "IScreen.h"

#include "FractalMgr.h"

class AppClient;

class ClientMainScreen : public IScreen
{
public:
    ClientMainScreen(AppClient &parent);
    ~ClientMainScreen();

    virtual void Build() override;

    virtual void Destroy() override;

    virtual void OnEntry() override;

    virtual void OnExit() override;

    virtual void Update() override;

    virtual void Draw() override;

    virtual int GetNextScreenIndex() const override;

    virtual int GetPreviousScreenIndex() const override;

private:
    AppClient &m_parent;

    FractalMgr m_fractalMgr;

    sfg::Window::Ptr m_guiWindow;
    sfg::Adjustment::Ptr m_adjustmentJuliaCr;
    sfg::Adjustment::Ptr m_adjustmentJuliaCi;
    sfg::Label::Ptr m_labelFPS;
    sfg::Label::Ptr m_labelFrametime;

    float m_updatePerformanceLabelsTimer;
};