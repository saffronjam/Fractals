#include "FractalMgr.h"

FractalMgr::FractalMgr()
    : m_lastViewport(vl::Null<>(), vl::Null<>())
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
        m_fractalSets.at(m_activeFractalSet)->ReconstructImage();
    }
}

void FractalMgr::Draw()
{
    m_fractalSets.at(m_activeFractalSet)->Draw();
}

void FractalMgr::SetFractal(const std::string &fractal)
{
    m_activeFractalSet = fractal;
    // m_fractalSets.at(m_activeFractalSet)->ReconstructImage();
}

void FractalMgr::SetIterationCount(size_t iterations)
{
    for (auto &[name, fractalSet] : m_fractalSets)
    {
        fractalSet->SetComputeIteration(iterations);
        fractalSet->Start(m_lastViewport);
    }
}
