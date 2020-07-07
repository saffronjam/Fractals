#pragma once

#include "FractalSet.h"

class Mandelbrot : public FractalSet
{
public:
    Mandelbrot();

private:
    struct MandelbrotWorker : public FractalSet::Worker
    {
        void Compute() override;
    };
};