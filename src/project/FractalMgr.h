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

    void Start();

    void SetFractal(const std::string &fractal);

protected:
    void ComputeFractals();

private:
    std::map<std::string, FractalSet *> m_fractalSets;
    std::string m_activeFractalSet;
};