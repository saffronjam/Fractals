#pragma once

#include <string>
#include <map>
#include <mutex>

#include <SFGUI/Adjustment.hpp>

#include "Mandelbrot.h"
#include "Julia.h"

class FractalMgr
{
public:
    FractalMgr();
    ~FractalMgr();

    void Update(const sfg::Adjustment::Ptr &cr, const sfg::Adjustment::Ptr &ci);
    void Draw();

    [[nodiscard]] const std::map<std::string, FractalSet *> &GetFractalSets() const noexcept { return m_fractalSets; }

    void SetFractalSet(const std::string &fractal);
    void SetComputeIterationCount(size_t iterations);
    void SetJuliaC(const std::complex<double> &c);
    void SetPalette(FractalSet::Palette palette);
    void SetMandelbrotState(Mandelbrot::State state);
    void SetJuliaState(Julia::State state);

private:
    std::map<std::string, FractalSet *> m_fractalSets;
    std::string m_activeFractalSet;

    std::pair<sf::Vector2f, sf::Vector2f> m_lastViewport;
};