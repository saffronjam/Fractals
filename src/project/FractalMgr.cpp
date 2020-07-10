#include "FractalMgr.h"

FractalMgr::FractalMgr()
        : m_lastViewport(vl::Null<>(), vl::Null<>())
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

void FractalMgr::Update(const sfg::Adjustment::Ptr &cr, const sfg::Adjustment::Ptr &ci)
{
    if (m_lastViewport != Camera::GetViewport())
    {
        m_lastViewport = Camera::GetViewport();
        m_fractalSets.at(m_activeFractalSet)->SetSimBox(m_lastViewport);
        m_fractalSets.at(m_activeFractalSet)->MarkForImageRecompute();
        m_fractalSets.at(m_activeFractalSet)->MarkForImageReconstruct();
    }
    m_fractalSets[m_activeFractalSet]->Update();

    if (m_activeFractalSet == "Julia")
    {
        auto juliaSet = dynamic_cast<Julia *>(m_fractalSets.at(m_activeFractalSet));
        if (juliaSet)
        {
            ci->SetValue(static_cast<float>(juliaSet->GetC().imag() + 2.5f));
            cr->SetValue(static_cast<float>(juliaSet->GetC().real() + 2.5f));
        }
    }
}

void FractalMgr::Draw()
{
    m_fractalSets.at(m_activeFractalSet)->Draw();
}

void FractalMgr::SetFractalSet(const std::string &fractal)
{
    m_activeFractalSet = fractal;
    m_fractalSets.at(m_activeFractalSet)->MarkForImageRecompute();
    m_fractalSets.at(m_activeFractalSet)->MarkForImageReconstruct();
    // Nullify the viewport cache to force a new viewport to be computed
    m_lastViewport = std::make_pair(vl::Null<>(), vl::Null<>());
}

void FractalMgr::SetComputeIterationCount(size_t iterations)
{
    for (auto&[name, fractalSet] : m_fractalSets)
    {
        fractalSet->SetComputeIterationCount(iterations);
        fractalSet->MarkForImageRecompute();
        fractalSet->MarkForImageReconstruct();
    }
}

void FractalMgr::SetJuliaC(const std::complex<double> &c)
{
    auto juliaSet = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
    if (juliaSet)
    {
        juliaSet->SetC(c, true);
        juliaSet->MarkForImageReconstruct();
    }
}

void FractalMgr::SetJuliaCR(double r)
{
    auto juliaSet = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
    if (juliaSet)
    {
        juliaSet->SetCR(r, true);
        juliaSet->MarkForImageReconstruct();
    }
}

void FractalMgr::SetJuliaCI(double i)
{
    auto juliaSet = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
    if (juliaSet)
    {
        juliaSet->SetCI(i, true);
        juliaSet->MarkForImageReconstruct();
    }
}

void FractalMgr::SetPalette(FractalSet::Palette palette)
{
    for (auto&[name, fractalSet] : m_fractalSets)
    {
        fractalSet->SetPalette(palette);
        fractalSet->MarkForImageReconstruct();
    }
}

void FractalMgr::SetMandelbrotState(Mandelbrot::State state)
{
    auto mandelbrotSet = dynamic_cast<Mandelbrot *>(m_fractalSets["Mandelbrot"]);
    if (mandelbrotSet)
    {
        mandelbrotSet->SetState(state);
    }
}

void FractalMgr::SetJuliaState(Julia::State state)
{
    auto juliaSet = dynamic_cast<Julia *>(m_fractalSets["Julia"]);
    if (juliaSet)
    {
        juliaSet->SetState(state);
    }
}
