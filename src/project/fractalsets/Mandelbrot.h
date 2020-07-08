#pragma once

#include "FractalSet.h"

class Mandelbrot : public FractalSet
{
public:
    Mandelbrot();

    sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations = 1);

private:
    struct MandelbrotWorker : public FractalSet::Worker
    {
        void Compute() override;
    };
};