#include "Mandelbrot.h"

std::atomic<size_t> Mandelbrot::m_nWorkerComplete = 0;

Mandelbrot::Mandelbrot()
    : FractalSet("Mandelbrot"),
      m_workers(32)
{
    for (auto &worker : m_workers)
    {
        worker.alive = true;
        worker.fractalArray = m_fractalArray;
        worker.nWorkers = m_workers.size();
        worker.screenWidth = Window::GetWidth();
        worker.thread = std::thread(&Mandelbrot::Worker::Compute, &worker);
    }
}

void Mandelbrot::Start(const std::pair<sf::Vector2f, sf::Vector2f> &viewport)
{
    double imageSectionWidth = static_cast<double>(Window::GetWidth()) / static_cast<double>(m_workers.size());
    double fractalSectionWidth = static_cast<double>(viewport.second.x - viewport.first.x) / static_cast<double>(m_workers.size());
    m_nWorkerComplete = 0;

    for (size_t i = 0; i < m_workers.size(); i++)
    {
        m_workers[i].imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
        m_workers[i].imageBR = sf::Vector2<double>(imageSectionWidth * (i + 1), Window::GetHeight());
        m_workers[i].fractalTL = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * i), viewport.first.y);
        m_workers[i].fractalBR = sf::Vector2<double>(viewport.first.x + static_cast<double>(fractalSectionWidth * (i + 1)), viewport.second.y);
        m_workers[i].iterations = m_computeIterations;

        std::unique_lock<std::mutex> lm(m_workers[i].mutex);
        m_workers[i].cvStart.notify_one();
    }

    while (m_nWorkerComplete < 1) // Wait for all workers to complete
    {
    }
}

sf::Vector2f Mandelbrot::TranslatePoint(const sf::Vector2f &point, int iterations)
{
    std::complex<float> c(point.x, point.y);
    std::complex<float> z(0.0f, 0.0f);

    for (int n = 0; n < iterations && abs(z) < 2.0; n++)
        z = (z * z) + c;

    return sf::Vector2f(z.real(), z.imag());
}

void Mandelbrot::Worker::Compute()
{
    while (alive)
    {
        std::unique_lock<std::mutex> lm(mutex);
        cvStart.wait(lm);

        double xScale = (fractalBR.x - fractalTL.x) / (imageBR.x - imageTL.x);
        double yScale = (fractalBR.y - fractalTL.y) / (imageBR.y - imageTL.y);

        double y_pos = fractalTL.y;

        int y_offset = 0;
        int row_size = screenWidth;

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

        for (y = imageTL.y; y < imageBR.y; y++)
        {
            // Reset x_position
            _a = SIMD_SetOne(fractalTL.x);
            _x_pos = SIMD_Add(_a, _x_pos_offsets);

            _ci = SIMD_SetOne(y_pos);

            for (x = imageTL.x; x < imageBR.x; x += 4)
            {
                _cr = _x_pos;
                _zr = SIMD_SetZero();
                _zi = SIMD_SetZero();
                _n = SIMD_SetZero256i();

            repeat:
                _zr2 = SIMD_Mul(_zr, _zr);
                _zi2 = SIMD_Mul(_zi, _zi);
                _a = SIMD_Sub(_zr2, _zi2);
                _a = SIMD_Add(_a, _cr);
                _b = SIMD_Mul(_zr, _zi);
                _b = SIMD_MulAdd(_b, _two, _ci);
                _zr = _a;
                _zi = _b;
                _a = SIMD_Add(_zr2, _zi2);
                _mask1 = SIMD_LessThan(_a, _four);
                _mask2 = SIMD_GreaterThani(_iterations, _n);
                _mask2 = SIMD_Andi(_mask2, SIMD_CastToInt(_mask1));
                _c = SIMD_Andi(_one, _mask2); // Zero out ones where n < iterations
                _n = SIMD_Addi(_n, _c);       // n++ Increase all n
                if (SIMD_MoveMask(SIMD_CastToFloat(_mask2)) > 0)
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

        m_nWorkerComplete++;
    }
}