#include "FractalMgr.h"

FractalMgr::FractalMgr()
{
    m_fractalSets.emplace(std::make_pair("Mandelbrot", new Mandelbrot()));

    m_activeFractalSet = "Mandelbrot";
}

FractalMgr::~FractalMgr()
{
    for (auto &[name, set] : m_fractalSets)
    {
        delete set;
        set = nullptr;
    }
}

void FractalMgr::Update()
{
    if (m_lastViewport != Camera::GetViewport())
    {
        m_lastViewport = Camera::GetViewport();
        m_fractalSets.at(m_activeFractalSet)->Start(m_lastViewport);

        log_info("rect: tl:%f,%f  br:%f,%f", m_lastViewport.first.x, m_lastViewport.first.y, m_lastViewport.second.x, m_lastViewport.second.y);
    }
}

void FractalMgr::Draw()
{
    m_fractalSets.at(m_activeFractalSet)->Draw();
}