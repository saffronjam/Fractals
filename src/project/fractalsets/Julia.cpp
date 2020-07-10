#include "Julia.h"

Julia::Julia()
        : FractalSet("Julia"),
          m_desiredC(0.0, 0.0),
          m_currentC(0.0, 0.0),
          m_startC(0.0, 0.0),
          m_animationTimer(0.0f),
          m_cTransitionTimer(0.0f),
          m_cTransitionDuration(0.5f),
          m_state(State::None)
{
    for (int i = 0; i < 32; i++)
    {
        AddWorker(new JuliaWorker);
    }
}

void Julia::Update()
{
    if (m_cTransitionTimer <= m_cTransitionDuration && m_state == State::None)
    {
        float delta = (std::sin((m_cTransitionTimer / m_cTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
        m_currentC.real(m_startC.real() + static_cast<double>(delta) * (m_desiredC.real() - m_startC.real()));
        m_currentC.imag(m_startC.imag() + static_cast<double>(delta) * (m_desiredC.imag() - m_startC.imag()));
        MarkForImageRecompute();
        MarkForImageReconstruct();
        m_cTransitionTimer += Clock::Delta().asSeconds();
    }
    else
    {
        m_currentC = m_desiredC;
    }
    for (auto &worker : m_workers)
    {
        auto juliaWorker = dynamic_cast<JuliaWorker *>(worker);
        juliaWorker->c = m_currentC;
    }

    switch (m_state)
    {
    case Julia::State::Animate:
    {
        double x = 0.7885 * std::cos(m_animationTimer);
        double y = 0.7885 * std::sin(m_animationTimer);
        SetC(std::complex<double>(x, y));
        m_animationTimer += Clock::Delta().asSeconds() / 2.0f;
        if (m_animationTimer > 2.0f * PI<>)
            m_animationTimer = 0.0f;
        break;
    }
    case Julia::State::FollowCursor:
    {
        auto mousePos = Camera::ScreenToWorld(Mouse::GetPos());
        SetC(std::complex<double>(mousePos.x, mousePos.y));
        break;
    }
    default:
        break;
    }

    FractalSet::Update();
}

void Julia::SetC(const std::complex<double> &c)
{
    m_desiredC = c;
    m_startC = m_currentC;
    m_cTransitionTimer = 0.0f;
}

void Julia::JuliaWorker::Compute()
{
    while (alive)
    {
        std::unique_lock<std::mutex> lm(mutex);
        cvStart.wait(lm);
        if (!alive)
        {
            nWorkerComplete++;
            return;
        }

        double xScale = (fractalBR.x - fractalTL.x) / (imageBR.x - imageTL.x);
        double yScale = (fractalBR.y - fractalTL.y) / (imageBR.y - imageTL.y);

        double y_pos = fractalTL.y;

        int y_offset = 0;
        int row_size = simWidth;

        int x, y;

        SIMD_Double _a, _b, _two, _four, _mask1;
        SIMD_Double _zr, _zi, _zr2, _zi2, _cr, _ci;
        SIMD_Double _x_pos_offsets, _x_pos, _x_scale, _x_jump;
        SIMD_Integer _one, _c, _n, _iterations, _mask2;

        _one = SIMD_SetOnei(1);
        _two = SIMD_SetOne(2.0);
        _four = SIMD_SetOne(4.0);
        _iterations = SIMD_SetOnei(iterations);

        _x_scale = SIMD_SetOne(xScale);
        _x_jump = SIMD_SetOne(xScale * 4.0);
        _x_pos_offsets = SIMD_Set(0.0, 1.0, 2.0, 3.0);
        _x_pos_offsets = SIMD_Mul(_x_pos_offsets, _x_scale);

        _cr = SIMD_SetOne(c.real());
        _ci = SIMD_SetOne(c.imag());

        for (y = imageTL.y; y < imageBR.y; y++)
        {
            // Reset x_position
            _a = SIMD_SetOne(fractalTL.x);
            _x_pos = SIMD_Add(_a, _x_pos_offsets);

            for (x = imageTL.x; x < imageBR.x; x += 4)
            {
                _zr = _x_pos;
                _zi = SIMD_SetOne(y_pos);
                _n = SIMD_SetZero256i();

                repeat:
                _zr2 = SIMD_Mul(_zr, _zr);
                _zi2 = SIMD_Mul(_zi, _zi);
                _a = SIMD_Sub(_zr2, _zi2);
                _a = SIMD_Add(_a, _cr);
                _b = SIMD_Mul(_zr, _zi);
                _b = SIMD_Mul(_b, _two);
                _b = SIMD_Add(_b, _ci);
                _zr = _a;
                _zi = _b;
                _a = SIMD_Add(_zr2, _zi2);
                _mask1 = SIMD_LessThan(_a, _four);
                _mask2 = SIMD_GreaterThani(_iterations, _n);
                _mask2 = SIMD_Andi(_mask2, SIMD_CastToInt(_mask1));
                _c = SIMD_Andi(_one, _mask2); // Zero out ones where n < iterations
                _n = SIMD_Addi(_n, _c);       // n++ Increase all n
                if (SIMD_SignMask(SIMD_CastToFloat(_mask2)) > 0)
                    goto repeat;

                fractalArray[y_offset + x + 0] = static_cast<int>(_n[3]);
                fractalArray[y_offset + x + 1] = static_cast<int>(_n[2]);
                fractalArray[y_offset + x + 2] = static_cast<int>(_n[1]);
                fractalArray[y_offset + x + 3] = static_cast<int>(_n[0]);

                _x_pos = SIMD_Add(_x_pos, _x_jump);
            }

            y_pos += yScale;
            y_offset += row_size;
        }

        (*nWorkerComplete)++;
    }
}