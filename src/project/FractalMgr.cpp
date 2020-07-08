#include "FractalMgr.h"

FractalMgr::FractalMgr()
    : m_lastViewport(vl::Null<>(), vl::Null<>()),
      m_iterations(64)
{
    m_fractalSets.emplace(std::make_pair("Mandelbrot", new Mandelbrot()));
    m_fractalSets.emplace(std::make_pair("Julia", new Julia()));

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
    auto newViewPort = Camera::GetViewport();
    if (m_lastViewport != newViewPort)
    {
        m_lastViewport = newViewPort;
        m_fractalSets.at(m_activeFractalSet)->Start(m_lastViewport);
        m_fractalSets.at(m_activeFractalSet)->ReconstructImage();
    }
}

void FractalMgr::Draw()
{
    m_fractalSets.at(m_activeFractalSet)->Draw();

    if (Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::GetPos().x < Window::GetWidth() - 200)
    {
        sf::Vector2f start = Camera::ScreenToWorld(Mouse::GetPos());
        sf::Vector2f to = start;

        for (int i = 1; i < m_iterations; i++)
        {
            sf::Vector2f from = m_fractalSets[m_activeFractalSet]->TranslatePoint(start, i);
            Camera::DrawLine(from, to, sf::Color(200, 200, 200, 60));
            to = from;
            Camera::DrawPoint(to, sf::Color(255, 255, 255, 150));
        }
    }
}

void FractalMgr::SetFractalSet(const std::string &fractal)
{
    m_activeFractalSet = fractal;
    m_fractalSets.at(m_activeFractalSet)->Start(m_lastViewport);
    m_fractalSets.at(m_activeFractalSet)->ReconstructImage();
}

void FractalMgr::SetIterationCount(size_t iterations)
{
    for (auto &[name, fractalSet] : m_fractalSets)
    {
        fractalSet->SetComputeIteration(iterations);
        fractalSet->Start(m_lastViewport);
        fractalSet->ReconstructImage();
    }
    m_iterations = iterations;
}

void FractalMgr::SetJuliaC(const std::complex<double> c)
{
    auto julia = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
    if (julia != nullptr)
    {
        julia->SetC(c);
        if (m_activeFractalSet == "Julia")
        {
            m_fractalSets.at("Julia")->Start(m_lastViewport);
            m_fractalSets.at("Julia")->ReconstructImage();
        }
    }
}