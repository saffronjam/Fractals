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

    void Update(sfg::Adjustment::Ptr cr, sfg::Adjustment::Ptr ci);
    void Draw();

    const std::map<std::string, FractalSet *> &GetFractalSets() const noexcept { return m_fractalSets; }

    void SetFractalSet(const std::string &fractal);
    void SetIterationCount(size_t iterations);
    void SetJuliaC(const std::complex<double> &c);
    void SetDrawComplexLines(bool onoff) noexcept { m_drawComplexLines = onoff; }
    void SetPalette(FractalSet::Palette palette);
    void SetJuliaSetState(Julia::State state);

private:
    // Used to only make the GUI send updates ONCE every frame so that the adjustment bars doesn't freeze the screen
    bool m_updatedThisFrame;

    std::map<std::string, FractalSet *> m_fractalSets;
    std::string m_activeFractalSet;

    std::pair<sf::Vector2f, sf::Vector2f> m_lastViewport;

    bool m_drawComplexLines;

    std::complex<double> m_juliaC;
    std::complex<double> m_juliaCGoal;
    float m_animationTimer;

    FractalSet::Palette m_palette;
    FractalSet::Palette m_desiredPalette;

    // Cached
    size_t m_iterations;
    size_t m_iterationsGoal;
};