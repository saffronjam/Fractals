#pragma once

#include "FractalSet.h"

class Mandelbrot : public FractalSet
{
public:
    enum class State
    {
        ComplexLines,
        None
    };

public:
    Mandelbrot();
    ~Mandelbrot() override = default;

    void Draw() override;

    static sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations);

    void SetState(State state) noexcept { m_state = state; }

private:
    State m_state;

private:
    struct MandelbrotWorker : public FractalSet::Worker
    {
        ~MandelbrotWorker() override = default;
        void Compute() override;
    };
};