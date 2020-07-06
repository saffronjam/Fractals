#include "FractalMgr.h"

FractalMgr::FractalMgr()
{
    m_fractalSets.emplace(std::make_pair("Mandlebrot", new Mandelbrot()));

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

void FractalMgr::Start()
{
}

void FractalMgr::ComputeFractals()
{
    sf::FloatRect computeRect = Camera::GetViewPort();
    m_fractalSets.at(m_activeFractalSet)->Start(computeRect);
}