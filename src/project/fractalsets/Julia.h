#pragma once

#include "FractalSet.h"

class Julia : public FractalSet
{
public:
    Julia();

    sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations = 1) override;

    void SetC(const std::complex<double> &c) noexcept { m_c = c; }

private:
    static std::complex<double> m_c;

private:
    struct JuliaWorker : public FractalSet::Worker
    {
        void Compute() override;
    };
};