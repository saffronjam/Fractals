#include "FractalMgr.h"

FractalMgr::FractalMgr()
    : m_updatedThisFrame(false),
      m_lastViewport(vl::Null<>(), vl::Null<>()),
      m_iterations(64),
      m_drawComplexLines(false),
      m_animationTimer(0.0f)
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

void FractalMgr::Update(sfg::Adjustment::Ptr cr, sfg::Adjustment::Ptr ci)
{
    m_updatedThisFrame = false;
    auto newViewPort = Camera::GetViewport();
    if (m_lastViewport != newViewPort)
    {
        m_lastViewport = newViewPort;
        m_fractalSets.at(m_activeFractalSet)->Start(m_lastViewport);
        m_fractalSets.at(m_activeFractalSet)->ReconstructImage();
    }

    if (m_activeFractalSet == "Julia")
    {
        switch (m_juliaState)
        {
        case JuliaState::Animate:
        {
            double x = 0.7885 * std::cos(m_animationTimer);
            double y = 0.7885 * std::sin(m_animationTimer);
            SetJuliaC(std::complex<double>(x, y));
            cr->SetValue(x + 2.5);
            ci->SetValue(y + 2.5);
            m_animationTimer += Clock::Delta().asSeconds() / 2.0f;
            if (m_animationTimer > 2.0f * PI<>)
                m_animationTimer = 0.0f;
            break;
        }
        case JuliaState::FollowCursor:
        {
            auto mousePos = Camera::ScreenToWorld(Mouse::GetPos());
            SetJuliaC(std::complex<double>(mousePos.x, mousePos.y));
            cr->SetValue(mousePos.x + 2.5);
            ci->SetValue(mousePos.y + 2.5);
            break;
        }
        case JuliaState::None:
        {
            break;
        }
        default:
        {
            break;
        }
        }
    }

    if (!m_updatedThisFrame && m_iterations != m_iterationsGoal)
    {
        m_iterations = m_iterationsGoal;
        m_updatedThisFrame = true;
        for (auto &[name, fractalSet] : m_fractalSets)
        {
            fractalSet->SetComputeIteration(m_iterations);
            fractalSet->Start(m_lastViewport);
            fractalSet->ReconstructImage();
        }
    }
    if (m_activeFractalSet == "Julia" && !m_updatedThisFrame && m_juliaC != m_juliaCGoal)
    {
        auto julia = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
        if (julia != nullptr)
        {
            m_juliaC = m_juliaCGoal;
            m_updatedThisFrame = true;
            julia->SetC(m_juliaC);
            if (m_activeFractalSet == "Julia")
            {
                m_fractalSets.at("Julia")->Start(m_lastViewport);
                m_fractalSets.at("Julia")->ReconstructImage();
            }
        }
    }
}

void FractalMgr::Draw()
{
    m_fractalSets.at(m_activeFractalSet)->Draw();

    if (m_activeFractalSet == "Mandelbrot" && m_drawComplexLines && Mouse::GetPos().x < Window::GetWidth() - 200)
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
    m_iterationsGoal = iterations;
}

void FractalMgr::SetJuliaC(const std::complex<double> c)
{
    m_juliaCGoal = c;
}