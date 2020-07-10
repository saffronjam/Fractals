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
    ~Julia() override = default;

    void Update() override;

    [[nodiscard]] const std::complex<double> &GetC() const noexcept { return m_desiredC; }

    void SetState(State state) noexcept { m_state = state; }
    void SetC(const std::complex<double> &c, bool animate = false);
    void SetCR(double r, bool animate = false) { SetC(std::complex(r, GetC().imag()),animate); }
    void SetCI(double i, bool animate = false) { SetC(std::complex(GetC().real(),i),animate); }

private:
    State m_state;

    std::complex<double> m_desiredC;
    std::complex<double> m_currentC;
    std::complex<double> m_startC;

    float m_animationTimer;

    float m_cTransitionTimer;
    float m_cTransitionDuration;

private:
    struct JuliaWorker : public FractalSet::Worker
    {
        ~JuliaWorker() override = default;
        void Compute() override;

        std::complex<double> c;
    };
};