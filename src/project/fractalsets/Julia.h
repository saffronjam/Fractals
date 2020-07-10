#pragma once

#include "FractalSet.h"

class Julia : public FractalSet
{
public:
    enum class State
    {
        Animate,
        FollowCursor,
        None
    };

public:
    Julia();

    void Update() override;

    sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations = 1) override;

    State GetState() const noexcept { return m_state; }

    void SetState(State state) noexcept { m_state = state; }
    void SetC(const std::complex<double> &c);

private:
    State m_state;

    std::complex<double> m_desiredC;

    static std::complex<double> m_currentC;
    std::complex<double> m_startC;
    float m_cTransitionTimer;
    float m_cTransitionDuration;

private:
    struct JuliaWorker : public FractalSet::Worker
    {
        void Compute() override;
    };
};