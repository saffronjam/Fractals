#pragma once

#include <string>
#include <map>
#include <mutex>

#include "Mandelbrot.h"
#include "Julia.h"

class FractalMgr
{
public:
    FractalMgr();
    ~FractalMgr();

    void Update();
    void Draw();

    const std::map<std::string, FractalSet *> &GetFractalSets() const noexcept { return m_fractalSets; }

    void SetFractalSet(const std::string &fractal);
    void SetIterationCount(size_t iterations);

private:
    std::map<std::string, FractalSet *> m_fractalSets;
    std::string m_activeFractalSet;

    std::pair<sf::Vector2f, sf::Vector2f> m_lastViewport;

    // Cached
    size_t m_iterations;
};