#pragma once

#include <string>
#include <map>
#include <mutex>

#include "Mandelbrot.h"

class FractalMgr
{
public:
    FractalMgr();
    ~FractalMgr();

    void Update();
    void Draw();

    void SetFractal(const std::string &fractal);

private:
    std::map<std::string, FractalSet *> m_fractalSets;
    std::string m_activeFractalSet;

    std::pair<sf::Vector2f, sf::Vector2f> m_lastViewport;
};